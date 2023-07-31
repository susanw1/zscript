package org.zcode.javareceiver.modules;

import org.zcode.javareceiver.core.ZcodeOutStream;
import org.zcode.javareceiver.core.ZcodeStatus;
import org.zcode.javareceiver.execution.ZcodeAddressingContext;
import org.zcode.javareceiver.execution.ZcodeCommandContext;
import org.zcode.javareceiver.tokenizer.Zchars;

public interface ZcodeModule {

    int getModuleID();

    void execute(ZcodeCommandContext ctx, int command);

    default void moveAlong(ZcodeCommandContext ctx, int command) {
        ctx.status(ZcodeStatus.INTERNAL_ERROR);
    }

    default void address(ZcodeAddressingContext ctx) {
        ctx.status(ZcodeStatus.INTERNAL_ERROR);
    }

    default void addressMoveAlong(ZcodeAddressingContext ctx) {
        ctx.status(ZcodeStatus.INTERNAL_ERROR);
    }

    default boolean notification(ZcodeOutStream out, int i, boolean isAddressed) {
        out.asCommandOutStream().writeField(Zchars.Z_STATUS, ZcodeStatus.INTERNAL_ERROR);
        return true;
    }

}
