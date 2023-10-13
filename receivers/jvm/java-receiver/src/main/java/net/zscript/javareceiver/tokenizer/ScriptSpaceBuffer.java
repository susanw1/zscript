package net.zscript.javareceiver.tokenizer;

import static java.text.MessageFormat.format;

import java.util.Arrays;

public class ScriptSpaceBuffer extends TokenArrayBuffer {

    public ScriptSpaceBuffer() {
        super(new byte[0]);
        writeStart = 0;
    }

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

    public class ScriptSpaceWriterBuffer extends TokenArrayBuffer {
        private final ScriptSpaceBuffer parent;

        private ScriptSpaceWriterBuffer(ScriptSpaceBuffer parent, byte[] data) {
            super(data);
            this.parent = parent;
            offset(data.length, 1);
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

        public void commitChanges() {
            parent.data = Arrays.copyOf(data, writeStart);
            parent.writeStart = writeStart + 2;
        }
    };

    public ScriptSpaceWriterBuffer append() {
        ScriptSpaceWriterBuffer writer = new ScriptSpaceWriterBuffer(this, data);
        writer.writeStart = data.length;
        return writer;
    }

    public ScriptSpaceWriterBuffer fromStart() {
        ScriptSpaceWriterBuffer writer = new ScriptSpaceWriterBuffer(this, new byte[20]);
        return writer;
    }

    public int getCurrentLength() {
        return data.length;
    }

}
