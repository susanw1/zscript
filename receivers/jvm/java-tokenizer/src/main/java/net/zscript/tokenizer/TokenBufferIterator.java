package net.zscript.tokenizer;

import net.zscript.tokenizer.TokenBuffer.TokenReader.ReadToken;
import net.zscript.util.OptIterator;

/**
 * Commonly-used iterator for moving along through a TokenBuffer's tokens.
 */
public interface TokenBufferIterator extends OptIterator<ReadToken> {

    /**
     * Clears any items read thus far from the buffer up to (and including) the most recently-read item. The next call to {@link #next()} will just return whatever token would have
     * been returned (which is now the new 'first item'), so the actual iteration flow is unaffected. This method allows a Parser to say "I've finished with everything I've seen so
     * far", potentially allowing the writer side to use the newly freed space in the case of a ring-buffer.
     * <p/>
     * Caution: Note that calling this method affects the state of the underlying buffer, unlike other "read"-side operations. In particular, it could cause any retained ReadTokens
     * or other existing iterators to find themselves dangling, pointing into "writer"-space which is subject to change without notice by the Writer. Call with care!
     * <p/>
     * Generally, avoid retaining ReadTokens or holding multiple iterators.
     */
    void flushBuffer();

}
