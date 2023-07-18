package org.zcode.javareceiver.semanticParser;

import org.zcode.javareceiver.core.ZcodeLockSet;

/**
 * Defines methods accessed by ZcodeActions.
 */
public interface ParseState {
    void softFail();

    void error();

    void errorSent();

    void setCommandStarted();

    boolean hasEcho();

    int getEcho();

    void seqEndSent();

    ZcodeLockSet getLocks();

    void setLocked(boolean b);

    boolean isLocked();

    void closeParenSent();

    byte getSeqEndMarker();

    void clearFirstCommand();

    void clearNeedsAction();

}
