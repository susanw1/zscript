package net.zscript.javareceiver.execution;

import net.zscript.javareceiver.core.Zscript;
import net.zscript.javareceiver.core.OutStream;

public interface ZscriptAction {
    boolean isEmptyAction();

    void performAction(Zscript z, OutStream out);

    boolean canLock(Zscript z);

    boolean lock(Zscript z);

}
