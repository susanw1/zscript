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
        if (bigBig.getLength() == 0) {
            for (int i = 0; i < slots.length; i++) {
                if (!slots[i].isParsed()) {
                    targetSlot = slots[i];
                    break;
                }
            }
            if (targetSlot != null) {
                if (mostRecent != null && !mostRecent.isFullyParsed() && !mostRecent.hasFailed()) {
                    parse(targetSlot, mostRecent);
                } else {
                    if (mostRecent != null) {
                        mostRecent.unsetFailed();
                    }
                    mostRecent = null;
                    for (int i = 0; i < rcode.getChannels().length; i++) {
                        RCodeCommandChannel channel = rcode.getChannels()[i];
                        if (channel.hasCommandSequence() && !channel.getCommandSequence().isActive()) {
                            RCodeCommandSequence seq = channel.getCommandSequence();
                            if (!channel.getInStream().isLocked()) {
                                seq.getInStream().getSequenceIn().openCommandSequence();
                                mostRecent = seq;
                                parse(targetSlot, seq);
                                seq.setActive();
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    private void parse(RCodeCommandSlot slot, RCodeCommandSequence sequence) {
        if (!sequence.getInStream().getSequenceIn().hasNextChar()) {
            sequence.setFullyParsed(true);
            sequence.releaseInStream();
        } else {
            boolean worked = slot.parseSingleCommand(sequence.getInStream(), sequence);
            if (!worked) {
                if (slot.getStatus() != RCodeResponseStatus.SKIP_COMMAND) {
                    sequence.addLast(slot);
                }
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
