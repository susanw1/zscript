package org.zcode.javareceiver.semanticParser;

import org.zcode.javareceiver.core.Zcode;
import org.zcode.javareceiver.semanticParser.ZcodeSemanticAction.ActionType;

/**
 * Defines methods accessed by ZcodeActions.
 */
public interface ParseState {
    byte getErrorStatus();

    void actionPerformed(ActionType type);

    byte takeCurrentMarker();

    boolean hasSentStatus();

    boolean hasEcho();

    int getEcho();

    boolean canLock(Zcode zcode);

    boolean lock(Zcode zcode);

    void unlock(Zcode zcode);

    boolean isEmpty();
}
