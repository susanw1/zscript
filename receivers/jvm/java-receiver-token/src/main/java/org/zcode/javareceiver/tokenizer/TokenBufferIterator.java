package org.zcode.javareceiver.tokenizer;

import org.zcode.javareceiver.tokenizer.ZcodeTokenBuffer.TokenReader.ReadToken;

public interface TokenBufferIterator extends OptIterator<ReadToken> {

    /**
     * Empties the buffer up to the next item, so the previously read item is lost.
     */
    void flushBuffer();

}
