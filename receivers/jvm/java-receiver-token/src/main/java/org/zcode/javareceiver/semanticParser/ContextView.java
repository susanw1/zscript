package org.zcode.javareceiver.semanticParser;

import org.zcode.javareceiver.tokenizer.ZcodeTokenBuffer.TokenReader;

/**
 * Defines parser state methods accessed by Command/Address contexts.
 */
public interface ContextView {
    TokenReader getReader();

    void setCommandComplete(boolean b);

    boolean isCommandComplete();

    void activate();

    boolean isActivated();

    void setStatus(byte status);

    void needsAction();
}
