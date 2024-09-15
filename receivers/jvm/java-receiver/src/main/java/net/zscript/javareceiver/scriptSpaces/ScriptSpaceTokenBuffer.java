package net.zscript.javareceiver.scriptSpaces;

import javax.annotation.Nonnull;

import net.zscript.tokenizer.AbstractArrayTokenBuffer;

public class ScriptSpaceTokenBuffer extends AbstractArrayTokenBuffer {

    public ScriptSpaceTokenBuffer() {
        super(new byte[0], 0);
    }

    private int getAvailableWrite(int writeCursor) {
        return (writeCursor >= getReadStart() ? getDataSize() : 0) + getReadStart() - writeCursor - 1;
    }

    @Override
    protected boolean checkAvailableCapacityFrom(int writeCursor, int size) {
        return getAvailableWrite(writeCursor) >= size;
    }

    @Override
    protected int offsetOnOverflow(int overflowedOffset) {
        return overflowedOffset - getDataSize();
    }

    public ScriptSpaceWriterTokenBuffer append() {
        return new ScriptSpaceWriterTokenBuffer(this, getInternalData(), true);
    }

    public ScriptSpaceWriterTokenBuffer fromStart() {
        return new ScriptSpaceWriterTokenBuffer(this, new byte[20], false);
    }

    public int getCurrentLength() {
        return getDataSize();
    }

    @Nonnull
    @Override
    public TokenWriter getTokenWriter() {
        throw new UnsupportedOperationException();
    }

    /**
     * Special token buffer for collecting tokens to be written to a real ScriptSpaceTokenBuffer.
     */
    public static class ScriptSpaceWriterTokenBuffer extends AbstractArrayTokenBuffer {
        private final ScriptSpaceTokenBuffer parent;

        private ScriptSpaceWriterTokenBuffer(ScriptSpaceTokenBuffer parent, byte[] data, boolean append) {
            super(data, append ? data.length : 0);
            this.parent = parent;
            // ensure a reasonable amount of space
            if (getDataSize() < 32) {
                extendData(32 - getDataSize());
            }
        }

        @Override
        protected boolean checkAvailableCapacityFrom(int writeCursor, int size) {
            return true;
        }

        @Override
        protected int offsetOnOverflow(int overflowedOffset) {
            extendData(getDataSize() + overflowedOffset);
            return overflowedOffset;
        }

        public void commitChanges() {
            parent.importInternalDataFrom(this);
        }
    }
}
