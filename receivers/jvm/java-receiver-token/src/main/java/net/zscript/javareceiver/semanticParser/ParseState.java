package net.zscript.javareceiver.semanticParser;

import net.zscript.javareceiver.core.Zscript;
import net.zscript.javareceiver.semanticParser.SemanticAction.ActionType;

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

    boolean canLock(Zscript zscript);

    boolean lock(Zscript zscript);

    void unlock(Zscript zscript);

    boolean isEmpty();
}
