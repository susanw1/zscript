package org.zcode.javareceiver.modules;

import java.util.OptionalInt;

import org.zcode.javareceiver.core.ZcodeOutStream;
import org.zcode.javareceiver.core.ZcodeStatus;
import org.zcode.javareceiver.execution.ZcodeAddressingContext;
import org.zcode.javareceiver.execution.ZcodeCommandContext;
import org.zcode.javareceiver.tokenizer.Zchars;

public class ZcodeModuleRegistry {
    private static final int    MAX_SYSTEM_CMD = 0xF;
    private final ZcodeModule[] modules        = new ZcodeModule[0x1000];

    public void addModule(ZcodeModule m) {
        if (modules[m.getModuleID()] != null) {
            throw new IllegalStateException("Two modules have the same ID");
        }
        modules[m.getModuleID()] = m;
    }

    public void execute(ZcodeCommandContext ctx) {
        // commands are completable unless explicitly marked otherwise by a command
        ctx.commandComplete();
        if (ctx.isEmpty()) {
            ctx.silentSucceed();
            return;
        }
        OptionalInt value = ctx.getField(Zchars.Z_CMD);
        if (value.isEmpty()) {
            ctx.status(ZcodeStatus.COMMAND_NOT_FOUND);
            return;
        }
        int cmd = value.getAsInt();
        if ((cmd >> 4) >= 0x1000) {
            ctx.status(ZcodeStatus.FIELD_TOO_LONG);
            return;
        }
        if (cmd > MAX_SYSTEM_CMD && !ctx.isActivated()) {
            ctx.status(ZcodeStatus.NOT_ACTIVATED);
            return;
        }
        if (modules[cmd >> 4] == null) {
            ctx.status(ZcodeStatus.COMMAND_NOT_FOUND);
            return;
        }

        modules[cmd >> 4].execute(ctx, cmd & 0xF);
    }

    public void moveAlong(ZcodeCommandContext ctx) {
        ctx.commandComplete();
        int cmd = ctx.getField(Zchars.Z_CMD).getAsInt();
        modules[cmd >> 4].moveAlong(ctx, cmd & 0xF);
    }

    public void execute(ZcodeAddressingContext ctx) {
        if (!ctx.isActivated()) {
            ctx.status(ZcodeStatus.NOT_ACTIVATED);
            return;
        }
        int addr = ctx.getAddressSegments().next().get();

        ZcodeModule module = modules[addr];
        if (module == null) {
            ctx.status(ZcodeStatus.ADDRESS_NOT_FOUND);
            return;
        }
        ctx.setCommandComplete();
        module.address(ctx);
    }

    public void moveAlong(ZcodeAddressingContext ctx) {
        int addr = ctx.getAddressSegments().next().get();
        modules[addr].addressMoveAlong(ctx);
    }

    public boolean notification(ZcodeOutStream out, int type) {
        return modules[type >> 4].notification(out, type & 0xF);
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

    public ZcodeModule getModule(int i) {
        return modules[i];
    }

}
