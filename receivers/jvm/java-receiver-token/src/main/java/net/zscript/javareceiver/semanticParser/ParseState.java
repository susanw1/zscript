package net.zscript.javareceiver.semanticParser;

import net.zscript.javareceiver.core.Zcode;
import net.zscript.javareceiver.semanticParser.ZcodeSemanticAction.ActionType;

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
