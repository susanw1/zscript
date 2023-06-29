package org.zcode.javareceiver.tokenizer;

import java.util.Iterator;

public interface BufferIterator<T> extends Iterator<T> {
    /**
     * Empties the buffer up to the next item, so the previously read item is lost.
     */
    void FlushBuffer();
}
