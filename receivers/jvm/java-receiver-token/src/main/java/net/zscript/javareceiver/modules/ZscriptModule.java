package net.zscript.javareceiver.modules;

import net.zscript.javareceiver.core.OutStream;
import net.zscript.javareceiver.core.ZscriptStatus;
import net.zscript.javareceiver.execution.AddressingContext;
import net.zscript.javareceiver.execution.CommandContext;
import net.zscript.javareceiver.tokenizer.Zchars;

public interface ZscriptModule {

    int getModuleID();

    void execute(CommandContext ctx, int command);

    default void moveAlong(CommandContext ctx, int command) {
        ctx.status(ZscriptStatus.INTERNAL_ERROR);
    }

    default void address(AddressingContext ctx) {
        ctx.status(ZscriptStatus.INTERNAL_ERROR);
    }

    default void addressMoveAlong(AddressingContext ctx) {
        ctx.status(ZscriptStatus.INTERNAL_ERROR);
    }

    default boolean notification(OutStream out, int i, boolean isAddressed) {
        out.asCommandOutStream().writeField(Zchars.Z_STATUS, ZscriptStatus.INTERNAL_ERROR);
        return true;
    }

}
