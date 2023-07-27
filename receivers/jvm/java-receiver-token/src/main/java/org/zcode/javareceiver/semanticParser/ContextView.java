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

    /**
     * Allows a background/async command component to signal that the command's code needs scheduling to move it along.
     *
     * Example, an interrupt-driven serial writer might call {@link #notifyNeedsAction()} in its interrupt handler to indicate that the data was all sent correctly. The command
     * will be
     */
    void notifyNeedsAction();

    void silentSucceed();

    int getChannelIndex();
}
