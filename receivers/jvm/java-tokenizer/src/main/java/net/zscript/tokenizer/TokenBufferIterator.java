package net.zscript.tokenizer;

import net.zscript.tokenizer.TokenBuffer.TokenReader.ReadToken;
import net.zscript.util.OptIterator;

public interface TokenBufferIterator extends OptIterator<ReadToken> {

    /**
     * Empties the buffer up to (and including) the most recently-read item. The next call to {@link #next()} will return the new first item - which it would have returned anyway!
     */
    void flushBuffer();

}
