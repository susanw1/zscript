package net.zscript.tokenizer;

import java.util.Iterator;

public interface BlockIterator extends Iterator<Byte> {
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
}
