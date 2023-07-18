package org.zcode.javareceiver.semanticParser;

import org.zcode.javareceiver.tokenizer.ZcodeTokenBuffer.TokenReader;

/**
 * Defines parser state methods accessed by Command/Address contexts.
 */
public interface ContextView {
    TokenReader getReader();

    void setCommandCanComplete(boolean b);

    boolean commandCanComplete();

    void activate();

    boolean isActivated();

    void setStatus(byte status);

    void needsAction();
}
