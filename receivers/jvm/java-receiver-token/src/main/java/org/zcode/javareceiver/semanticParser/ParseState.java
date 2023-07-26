package org.zcode.javareceiver.semanticParser;

import org.zcode.javareceiver.core.Zcode;
import org.zcode.javareceiver.semanticParser.ZcodeAction.ActionType;

/**
 * Defines methods accessed by ZcodeActions.
 */
public interface ParseState {
    byte getErrorStatus();

    void actionPerformed(ActionType type);

    boolean hasSentStatus();

    boolean hasEcho();

    int getEcho();

    boolean canLock(Zcode zcode);

    boolean lock(Zcode zcode);

    void unlock(Zcode zcode);

}
