package com.wittsfamily.rcode.javareceiver.parsing;

import com.wittsfamily.rcode.javareceiver.RCode;
import com.wittsfamily.rcode.javareceiver.RCodeParameters;
import com.wittsfamily.rcode.javareceiver.RCodeResponseStatus;

public class RCodeParser {
    private final RCode rcode;
    private RCodeCommandSequence mostRecent;
    private RCodeCommandSlot[] slots;
    private RCodeBigField bigBig;

    public RCodeParser(RCode rcode, RCodeParameters params) {
        mostRecent = null;
        this.rcode = rcode;
        bigBig = new RCodeBigField(params, true);
        slots = new RCodeCommandSlot[params.slotNum];
        for (int i = 0; i < slots.length; i++) {
            slots[i] = new RCodeCommandSlot(rcode, params, bigBig);
        }
    }

    public void parseNext() {
        RCodeCommandSlot targetSlot = null;
        if (!bigBig.isInUse()) {
            for (int i = 0; i < slots.length; i++) {
                if (!slots[i].isParsed()) {
                    targetSlot = slots[i];
                    break;
                }
            }
            if (targetSlot != null) {
                if (mostRecent != null && mostRecent.canContinueParse()) {
                    parse(targetSlot, mostRecent);
                } else {
                    if (mostRecent != null) {
                        mostRecent.unsetFailed();
                        mostRecent.releaseInStream();
                        mostRecent.getChannel().unlock();
                    }
                    mostRecent = null;
                    for (int i = 0; i < rcode.getChannels().length; i++) {
                        RCodeCommandChannel channel = rcode.getChannels()[i];
                        if (channel.canLock() && channel.hasCommandSequence() && !channel.getCommandSequence().isActive()
                                && !channel.getInStream().getSequenceInStream().isLocked()) {
                            if (rcode.getConfigFailureState() == null) {
                                mostRecent = beginSequenceParse(targetSlot, channel);
                            } else {
                                channel.getInStream().getSequenceInStream().open();
                                channel.getInStream().getSequenceInStream().close();
                                report_failure(channel);
                            }
                            break;
                        }
                    }
                }
            }
        }
    }

    private void report_failure(RCodeCommandChannel channel) {
        if (!channel.getOutStream().isLocked()) {
            channel.getOutStream().lock();
            channel.getOutStream().openResponse(channel);
            channel.getOutStream().writeStatus(RCodeResponseStatus.SETUP_ERROR);
            channel.getOutStream().writeBigStringField(rcode.getConfigFailureState());
            channel.getOutStream().writeCommandSequenceSeperator();
            channel.getOutStream().close();
            channel.getOutStream().unlock();
        }
    }

    private RCodeCommandSequence beginSequenceParse(RCodeCommandSlot targetSlot, RCodeCommandChannel channel) {
        RCodeCommandSequence candidateSequence = channel.getCommandSequence();
        channel.lock();
        candidateSequence.acquireInStream().open();
        if (candidateSequence.parseRCodeMarkers()) { // returns false if a debug sequence, true otherwise
            candidateSequence.setActive();
            if (!candidateSequence.isFullyParsed()) {
                // This is the normal case - nothing unexpected has happened
                parse(targetSlot, candidateSequence);
                return candidateSequence;
            } else {
                // This is when the incoming command sequence (candidateSequence) is empty / has only markers
                candidateSequence.releaseInStream();
                channel.unlock();
                return null;
            }
        } else {
            // The received sequence is debug - we will ignore it
            candidateSequence.releaseInStream();
            candidateSequence.reset();
            channel.unlock();
            return null;
        }
    }

    private void parse(RCodeCommandSlot slot, RCodeCommandSequence sequence) {
        boolean worked = slot.parseSingleCommand(sequence.acquireInStream().getCommandInStream(), sequence);
        sequence.addLast(slot);
        if (!worked) {
            sequence.acquireInStream().close();
        }
        if (!worked || slot.isEndOfSequence()) {
            sequence.setFullyParsed(true);
            sequence.releaseInStream();
            sequence.getChannel().unlock();
        }
    }
}
