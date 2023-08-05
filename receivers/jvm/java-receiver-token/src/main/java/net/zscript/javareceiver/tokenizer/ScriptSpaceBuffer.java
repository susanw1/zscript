package net.zscript.javareceiver.tokenizer;

import static java.text.MessageFormat.format;

import java.util.Arrays;

public class ScriptSpaceBuffer extends TokenArrayBuffer {
    public ScriptSpaceBuffer() {
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

    public TokenBuffer getReadOnlyView() {
        return new TokenArrayBuffer(Arrays.copyOf(data, writeStart)) {

            private int getAvailableWrite(int writeCursor) {
                return (writeCursor >= readStart ? data.length : 0) + readStart - writeCursor - 1;
            }

            @Override
            protected boolean checkAvailableCapacityFrom(int writeCursor, int size) {
                return getAvailableWrite(writeCursor) >= size;
            }

            @Override
            int offset(final int index, final int offset) {
                if (index < 0 || offset < 0 || offset >= data.length) {
                    throw new IllegalArgumentException(format("Unexpected values [pos={}, offset={}]", index, offset));
                }
                return (index + offset) % data.length;
            }

            @Override
            public TokenWriter getTokenWriter() {
                throw new UnsupportedOperationException();
            }
        };
    }
}
