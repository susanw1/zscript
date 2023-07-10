package org.zcode.javareceiver.modules;

import org.zcode.javareceiver.execution.ZcodeAddressingContext;
import org.zcode.javareceiver.execution.ZcodeCommandContext;

public interface ZcodeModule {

    int getModuleID();

    void execute(ZcodeCommandContext ctx, int command);

    void moveAlong(ZcodeCommandContext ctx, int command);

    void address(ZcodeAddressingContext ctx);

    void addressMoveAlong(ZcodeAddressingContext ctx);

}
