package net.zscript.javareceiver.semanticparser;

import net.zscript.javareceiver.core.Zscript;
import net.zscript.javareceiver.semanticparser.SemanticAction.ActionType;

/**
 * Defines methods accessed by ZscriptActions.
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
