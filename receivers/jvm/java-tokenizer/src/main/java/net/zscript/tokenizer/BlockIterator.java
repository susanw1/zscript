package net.zscript.tokenizer;

import java.util.Iterator;
import java.util.NoSuchElementException;

import net.zscript.util.ByteString;

/**
 * Defines an iterator that works over buffers of bytes, revealing them byte at a time, or in contiguous blocks.
 */
public interface BlockIterator extends Iterator<Byte>, ByteString.ByteAppendable {
    /**
     * Returns as many bytes in the iteration as are contiguous in memory (would give a pointer into the iterated structure in C++).
     *
     * @return an array of as many bytes as are contiguous in the structure.
     */
    byte[] nextContiguous();

    /**
     * Returns as many bytes in the iteration as are contiguous in memory, up to maxLength elements (would give a pointer into the iterated structure in C++).
     *
     * @return an array of as many bytes (up to maxLength) as are contiguous in the structure.
     */
    byte[] nextContiguous(int maxLength);

    /**
     * Defines how to append the (remaining) bytes from this iterator to make a ByteString, in efficient contiguous blocks. Note that if iteration has already started, the bytes
     * already read will be absent. The iterator will be fully consumed.
     * </p>
     * As an ByteAppendable, it may be used like this:
     * <pre>{@code
     * BlockIterator iter = ...
     * ByteString b = ByteString.from(iter);
     * }</pre>
     *
     * @param builder the builder to append to
     */
    default void appendTo(ByteString.ByteStringBuilder builder) {
        while (hasNext()) {
            builder.appendRaw(nextContiguous());
        }
    }

    BlockIterator EMPTY = new BlockIterator() {
        @Override
        public Byte next() {
            throw new NoSuchElementException();
        }

        @Override
        public byte[] nextContiguous() {
            throw new NoSuchElementException();
        }

        @Override
        public byte[] nextContiguous(int maxLength) {
            throw new NoSuchElementException();
        }

        @Override
        public boolean hasNext() {
            return false;
        }
    };
}
