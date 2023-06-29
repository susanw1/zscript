package org.zcode.javareceiver.tokenizer;

import java.util.Iterator;

public interface DmaIterator extends Iterator<Byte> {
    /**
     * Returns the as many elements in the iteration as are contiguous in memory (would give a pointer into the iterated structure in C++).
     *
     * @return an array of as many elements as are contiguous in the structure.
     */
    byte[] nextContiguous();

    /**
     * Returns the as many elements in the iteration as are contiguous in memory, up to maxLength elements (would give a pointer into the iterated structure in C++).
     *
     * @return an array of as many elements (up to maxLength) as are contiguous in the structure.
     */
    byte[] nextContiguous(int maxLength);
}
