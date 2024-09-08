package net.zscript.javareceiver.execution;

import net.zscript.javareceiver.core.SequenceOutStream;
import net.zscript.javareceiver.core.Zscript;

public interface ZscriptAction {
    boolean isEmptyAction();

    void performAction(Zscript z, SequenceOutStream out);

    boolean canLock(Zscript z);

    boolean lock(Zscript z);

}
