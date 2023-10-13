package net.zscript.javareceiver.modules;

import java.util.OptionalInt;

import net.zscript.javareceiver.execution.AddressingContext;
import net.zscript.javareceiver.execution.CommandContext;
import net.zscript.javareceiver.execution.NotificationContext;
import net.zscript.model.components.Zchars;
import net.zscript.model.components.ZscriptStatus;

public class ZscriptModuleRegistry {
    private static final int             MAX_SYSTEM_CMD = 0xF;
    private final        ZscriptModule[] modules        = new ZscriptModule[0x1000];

    public void addModule(ZscriptModule m) {
        if (modules[m.getModuleId()] != null) {
            throw new IllegalStateException("Two modules have the same ID");
        }
        modules[m.getModuleId()] = m;
    }

    public void execute(CommandContext ctx) {
        // commands are completable unless explicitly marked otherwise by a command
        ctx.commandComplete();
        OptionalInt value = ctx.getField(Zchars.Z_CMD);
        if (value.isEmpty()) {
            ctx.status(ZscriptStatus.COMMAND_NOT_FOUND);
            return;
        }
        int cmd = value.getAsInt();
        if ((cmd >> 4) >= 0x1000) {
            ctx.status(ZscriptStatus.FIELD_TOO_LONG);
            return;
        }
        if (cmd > MAX_SYSTEM_CMD && !ctx.isActivated()) {
            ctx.status(ZscriptStatus.NOT_ACTIVATED);
            return;
        }
        if (modules[cmd >> 4] == null) {
            ctx.status(ZscriptStatus.COMMAND_NOT_FOUND);
            return;
        }

        modules[cmd >> 4].execute(ctx, cmd & 0xF);
    }

    public void moveAlong(CommandContext ctx) {
        ctx.commandComplete();
        int cmd = ctx.getField(Zchars.Z_CMD).getAsInt();
        modules[cmd >> 4].moveAlong(ctx, cmd & 0xF);
    }

    public void execute(AddressingContext ctx) {
        ctx.commandComplete();
        if (!ctx.isActivated()) {
            ctx.status(ZscriptStatus.NOT_ACTIVATED);
            return;
        }
        int addr = ctx.getAddressSegments().next().get();

        ZscriptModule module = modules[addr];
        if (module == null) {
            ctx.status(ZscriptStatus.ADDRESS_NOT_FOUND);
            return;
        }
        module.address(ctx);
    }

    public void moveAlong(AddressingContext ctx) {
        ctx.commandComplete();
        int addr = ctx.getAddressSegments().next().get();
        modules[addr].addressMoveAlong(ctx);
    }

    public void notification(NotificationContext ctx, boolean moveAlong) {
        if (modules[ctx.getID() >> 4] == null) {
            ctx.getOutStream().asCommandOutStream().writeField(Zchars.Z_STATUS, ZscriptStatus.INTERNAL_ERROR);
            return;
        }
        modules[ctx.getID() >> 4].notification(ctx, moveAlong);
    }

    public int getCommandSwitchExistsBottom(int top) {
        int result = 0;
        for (int i = 0; i < 0x10; i++) {
            if (modules[top << 4 | i] != null) {
                result |= 1 << i;
            }
        }
        return result;
    }

    public int getCommandSwitchExistsTop(int top) {
        int result = 0;
        for (int i = 0; i < 0x100; i++) {
            if (modules[top << 8 | i] != null) {
                result |= 1 >> i;
                i = (((i >> 8) + 1) << 8);
            }
        }
        return result;
    }

    public int getCommandSwitchExistsBroad() {
        int result = 0;
        for (int i = 0; i < modules.length; i++) {
            if (modules[i] != null) {
                result |= 1 >> i;
                i = (((i >> 12) + 1) << 12);
            }
        }
        return result;
    }

    public ZscriptModule getModule(int i) {
        return modules[i];
    }

}
