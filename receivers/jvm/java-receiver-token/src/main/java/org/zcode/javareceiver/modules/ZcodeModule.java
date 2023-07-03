package org.zcode.javareceiver.modules;

import org.zcode.javareceiver.execution.ZcodeCommandEnviroment;

public interface ZcodeModule {

    int getModuleID();

    void execute(ZcodeCommandEnviroment env, int command);

    void moveAlong(ZcodeCommandEnviroment env, int command);

}
