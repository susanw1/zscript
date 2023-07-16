package org.zcode.javareceiver.semanticParser;

import org.zcode.javareceiver.core.ZcodeLockSet;
import org.zcode.javareceiver.tokenizer.ZcodeTokenBuffer.TokenReader;

/**
 * Defines methods accessed by ZcodeActions and Command/Address contexts.
 */
public interface ParseState {
    TokenReader getReader();

    void clearFirstCommand();

    void softFail();

    void error();

    void errorSent();

    void activate();

    boolean isActivated();

    void setComplete(boolean b);

    boolean isComplete();

    void setStarted();

    boolean hasEcho();

    int getEcho();

    void seqEndSent();

    ZcodeLockSet getLocks();

    void setLocked(boolean b);

    boolean isLocked();

    void closeParenSent();

    byte getSeqEndMarker();

    void needsAction();

    void clearNeedsAction();
}
