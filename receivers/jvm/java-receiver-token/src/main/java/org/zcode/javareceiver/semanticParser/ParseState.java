package org.zcode.javareceiver.semanticParser;

import org.zcode.javareceiver.core.Zcode;
import org.zcode.javareceiver.semanticParser.ZcodeAction.ActionType;

/**
 * Defines methods accessed by ZcodeActions.
 */
public interface ParseState {
    void actionPerformed(ActionType type);

    void startCommand();

    boolean hasEcho();

    int getEcho();

    void seqEndSent();

    void closeParenSent();

    byte getSeqEndMarker();

    void clearNeedsAction();

    boolean canLock(Zcode zcode);

    boolean lock(Zcode zcode);

    void unlock(Zcode zcode);

}
