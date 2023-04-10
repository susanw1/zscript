package org.zcode.javareceiver.semanticParser;

import org.zcode.javareceiver.core.ZcodeOutputStream;
import org.zcode.javareceiver.tokenizer.Zchars;
import org.zcode.javareceiver.tokenizer.ZcodeTokenBuffer;
import org.zcode.javareceiver.tokenizer.ZcodeTokenIterator;

public class ZcodeCommandRunningSystem {
    private final ZcodeCommandSequence seq;
    private final ZcodeTokenBuffer     buffer;
    private final ZcodeOutputStream    out;

    private byte status = 0;

    public ZcodeCommandRunningSystem(ZcodeCommandSequence seq, ZcodeTokenBuffer buffer, ZcodeOutputStream out) {
        this.seq = seq;
        this.buffer = buffer;
        this.out = out;
    }

    public boolean getField(byte field, IntPointer p) {
        ZcodeTokenIterator iterator = new ZcodeTokenIterator();
        buffer.setIterator(iterator);
        while (iterator.isValid() && !Zchars.isSeparator(iterator.getCurrentKey())) {
            if (iterator.getCurrentKey() == field) {
                p.value = iterator.getNumericValue();
                return true;
            }
            iterator.next();
        }
        return false;
    }

    public void setupBigField(ZcodeBigFieldIterator iterator) {
        iterator.setup(buffer);
    }

    public void setComplete(boolean complete) {
        seq.isComplete = complete;
    }

    public void fail(byte status) {
        this.status = status;
    }

    public byte getStatus() {
        return status;
    }

    public ZcodeOutputStream getOutput() {
        return out;
    }
}
