package org.zcode.javareceiver.tokenizer;

import org.zcode.javareceiver.tokenizer.ZcodeTokenBuffer.TokenReader.ReadToken;

public interface TokenBufferIterator extends OptIterator<ReadToken> {

    /**
     * Empties the buffer up to (and including) the most recently-read item. The next call to {@link #next()} will return the new first item - which it would have returned anyway!
     */
    void flushBuffer();

}
