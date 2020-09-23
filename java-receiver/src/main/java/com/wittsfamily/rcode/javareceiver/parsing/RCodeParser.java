package com.wittsfamily.rcode.javareceiver.parsing;

import com.wittsfamily.rcode.javareceiver.RCode;
import com.wittsfamily.rcode.javareceiver.RCodeParameters;

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

    public static void eatWhitespace(RCodeInStream in) {
        char c = in.peek();
        while (in.hasNext() && (c == ' ' || c == '\t' || c == '\r')) {
            in.read();
            c = in.peek();
        }
    }

    public static boolean shouldEatWhitespace(RCodeInStream in) {
        RCodeLookaheadStream l = in.getLookahead();
        char c = l.read();
        while (c == ' ' || c == '\t' || c == '\r') {
            c = l.read();
        }
        return c != '&';
    }

    public void parseNext() {
        RCodeCommandSlot targetSlot = null;
        if (bigBig.getLength() == 0) {
            for (int i = 0; i < slots.length; i++) {
                if (!slots[i].isParsed()) {
                    targetSlot = slots[i];
                    break;
                }
            }
            if (targetSlot != null) {
                if (mostRecent != null && mostRecent.isActive() && !mostRecent.isFullyParsed() && !mostRecent.hasFailed()) {
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
                        if (channel.canLock() && channel.hasCommandSequence() && !channel.getCommandSequence().isActive()) {
                            RCodeCommandSequence seq = channel.getCommandSequence();
                            if (!channel.getInStream().isLocked()) {
                                channel.lock();
                                seq.getInStream().getSequenceIn().openCommandSequence();
                                if (seq.parseFlags()) {
                                    if (seq.isFullyParsed()) {
                                        channel.unlock();
                                        seq.setActive();
                                        seq.releaseInStream();
                                    } else {
                                        mostRecent = seq;
                                        parse(targetSlot, seq);
                                        seq.setActive();
                                    }
                                }
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    private void parse(RCodeCommandSlot slot, RCodeCommandSequence sequence) {
        boolean worked = slot.parseSingleCommand(sequence.getInStream(), sequence);
        if (!worked) {
            sequence.addLast(slot);
            sequence.getInStream().skipSequence();
            sequence.setFullyParsed(true);
            sequence.releaseInStream();
            sequence.getChannel().unlock();
        } else {
            sequence.addLast(slot);
            if (slot.getEnd() == '\n') {
                sequence.setFullyParsed(true);
                sequence.releaseInStream();
                sequence.getChannel().unlock();
            }
        }
    }
}
