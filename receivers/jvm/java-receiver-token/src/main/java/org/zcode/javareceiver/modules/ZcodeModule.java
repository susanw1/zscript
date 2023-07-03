package org.zcode.javareceiver.modules;

import org.zcode.javareceiver.execution.ZcodeCommandView;

public interface ZcodeModule {

    int getModuleID();

    void execute(ZcodeCommandView env, int command);

    void moveAlong(ZcodeCommandView env, int command);

}
