package com.wittsfamily.rcode.parsing;

import com.wittsfamily.rcode.RCode;
import com.wittsfamily.rcode.RCodeParameters;

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
        if (bigBig.getLength() == 0) {
            for (int i = 0; i < slots.length; i++) {
                if (!slots[i].isParsed()) {
                    targetSlot = slots[i];
                    break;
                }
            }
            if (targetSlot != null) {
                if (mostRecent != null && !mostRecent.isFullyParsed()) {
                    parse(targetSlot, mostRecent);
                } else {
                    mostRecent = null;
                    for (int i = 0; i < rcode.getChannels().length; i++) {
                        RCodeCommandChannel ch = rcode.getChannels()[i];
                        if (ch.hasCommandSequence() && ch.getCommandSequence().peekFirst() == null) {
                            RCodeCommandSequence seq = ch.getCommandSequence();
                            if (!ch.getInStream().isLocked()) {
                                seq.getInStream().getSequenceIn().openCommandSequence();
                                mostRecent = seq;
                                parse(targetSlot, seq);
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    private void parse(RCodeCommandSlot slot, RCodeCommandSequence sequence) {
        if (!sequence.getInStream().getSequenceIn().hasNext()) {
            sequence.setFullyParsed(true);
            sequence.releaseInStream();
        } else {
            boolean worked = slot.parseSingleCommand(sequence.getInStream(), sequence);
            if (!worked) {
                sequence.addLast(slot);
                sequence.getInStream().skipSequence();
                sequence.setFullyParsed(true);
                sequence.releaseInStream();
            } else {
                sequence.addLast(slot);
                if (slot.getEnd() == '\n' || !slot.getCommand().continueLocking(sequence.getChannel())) {
                    sequence.setFullyParsed(true);
                    sequence.releaseInStream();
                }
            }
        }
    }
}
