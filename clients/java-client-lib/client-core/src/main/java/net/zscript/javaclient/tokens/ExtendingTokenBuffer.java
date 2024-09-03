package net.zscript.javaclient.tokens;

import net.zscript.tokenizer.AbstractArrayTokenBuffer;

/**
 * A TokenBuffer that automatically expands on being filled, for handling Zscript responses in a Client.
 */
public class ExtendingTokenBuffer extends AbstractArrayTokenBuffer {
    public ExtendingTokenBuffer() {
        super(20);
    }

    /**
     * There's always room, as we auto-expand.
     *
     * @param writeCursor
     * @param size
     * @return always true
     */
    @Override
    protected boolean checkAvailableCapacityFrom(int writeCursor, int size) {
        return true;
    }

    /**
     * Expands the underlying array, and returns the offset we were trying for now that it will fit.
     * <p>
     * {@inheritDoc}
     *
     * @param overflowedOffset the offset we were trying to access
     * @return the overflowedOffset that was passed in, as there's now enough space
     */
    @Override
    protected int offsetOnOverflow(int overflowedOffset) {
        // expand to double current size
        extendData(getDataSize());
        return overflowedOffset;
    }
}
