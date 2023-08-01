package net.zscript.javareceiver.tokenizer;

import static java.text.MessageFormat.format;

import java.util.Arrays;

public class ZcodeTokenExtendingBuffer extends ZcodeTokenArrayBuffer {
    public ZcodeTokenExtendingBuffer() {
        super(20);
    }

    @Override
    protected boolean checkAvailableCapacityFrom(int writeCursor, int size) {
        return true;
    }

    @Override
    int offset(int index, int offset) {
        if (index < 0 || offset < 0) {
            throw new IllegalArgumentException(format("Unexpected values [pos={}, offset={}]", index, offset));
        }
        if (index + offset >= data.length) {
            int newSize = index + offset + data.length;
            data = Arrays.copyOf(data, newSize);
        }
        return index + offset;
    }
}
