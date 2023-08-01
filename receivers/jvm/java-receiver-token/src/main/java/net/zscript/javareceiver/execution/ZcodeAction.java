package net.zscript.javareceiver.execution;

import net.zscript.javareceiver.core.Zcode;
import net.zscript.javareceiver.core.ZcodeOutStream;

public interface ZcodeAction {
    boolean isEmptyAction();

    void performAction(Zcode z, ZcodeOutStream out);

    boolean canLock(Zcode z);

    boolean lock(Zcode z);

}
