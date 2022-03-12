package org.zcode.javareceiver.parsing;

import org.zcode.javareceiver.Zcode;
import org.zcode.javareceiver.ZcodeParameters;
import org.zcode.javareceiver.ZcodeResponseStatus;

public class ZcodeParser {
    private final Zcode              zcode;
    private ZcodeCommandSequence     mostRecent;
    private final ZcodeCommandSlot[] slots;
    private final ZcodeBigField      hugeField;

    public ZcodeParser(final Zcode zcode, final ZcodeParameters params) {
        mostRecent = null;
        this.zcode = zcode;
        hugeField = new ZcodeBigField(params, true);
        slots = new ZcodeCommandSlot[params.slotNum];
        for (int i = 0; i < slots.length; i++) {
            slots[i] = new ZcodeCommandSlot(zcode, params, hugeField);
        }
    }

    public void parseNext() {
        ZcodeCommandSlot targetSlot = null;
        if (!hugeField.isInUse()) {
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
                    for (int i = 0; i < zcode.getChannels().length; i++) {
                        final ZcodeCommandChannel channel = zcode.getChannels()[i];
                        if (channel.canLock() && channel.hasCommandSequence() && !channel.getCommandSequence().isActive()
                                && (!channel.hasInStream() || !channel.acquireInStream().getSequenceInStream().isLocked())) {
                            if (zcode.getConfigFailureState() == null) {
                                mostRecent = beginSequenceParse(targetSlot, channel);
                            } else {
                                channel.acquireInStream().getSequenceInStream().open();
                                channel.acquireInStream().getSequenceInStream().close();
                                reportFailure(channel);
                                channel.releaseInStream();
                            }
                            break;
                        }
                    }
                }
            }
        }
    }

    private void reportFailure(final ZcodeCommandChannel channel) {
        if (!channel.hasOutStream() || !channel.acquireOutStream().isLocked()) {
            channel.acquireOutStream().lock();
            channel.acquireOutStream().openResponse(channel);
            channel.acquireOutStream().writeStatus(ZcodeResponseStatus.SETUP_ERROR);
            channel.acquireOutStream().writeBigStringField(zcode.getConfigFailureState());
            channel.acquireOutStream().writeCommandSequenceSeperator();
            channel.acquireOutStream().close();
            channel.acquireOutStream().unlock();
            channel.releaseOutStream();
        }
    }

    private ZcodeCommandSequence beginSequenceParse(final ZcodeCommandSlot targetSlot, final ZcodeCommandChannel channel) {
        final ZcodeCommandSequence candidateSequence = channel.getCommandSequence();
        channel.lock();
        candidateSequence.acquireInStream().open();
        if (candidateSequence.parseZcodeMarkers()) { // returns false if a debug sequence, true otherwise
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

    private void parse(final ZcodeCommandSlot slot, final ZcodeCommandSequence sequence) {
        final boolean worked = slot.parseSingleCommand(sequence.acquireInStream().getCommandInStream(), sequence);
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
