package org.zcode.javareceiver.execution;

import org.zcode.javareceiver.core.Zcode;
import org.zcode.javareceiver.core.ZcodeOutStream;

public interface ZcodeAction {
    boolean isEmptyAction();

    void performAction(Zcode z, ZcodeOutStream out);

    boolean canLock(Zcode z);

    boolean lock(Zcode z);

}
