package net.zscript.javareceiver.modules;

import net.zscript.javareceiver.core.ZcodeOutStream;
import net.zscript.javareceiver.core.ZcodeStatus;
import net.zscript.javareceiver.execution.ZcodeAddressingContext;
import net.zscript.javareceiver.execution.ZcodeCommandContext;
import net.zscript.javareceiver.tokenizer.Zchars;

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
