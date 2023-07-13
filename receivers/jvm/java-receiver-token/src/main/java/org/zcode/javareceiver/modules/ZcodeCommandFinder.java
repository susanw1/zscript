package org.zcode.javareceiver.modules;

import java.util.Optional;

import org.zcode.javareceiver.core.ZcodeStatus;
import org.zcode.javareceiver.execution.ZcodeCommandContext;

public class ZcodeCommandFinder {
    private static final ZcodeModule[] modules = new ZcodeModule[0x1000];

    private static final int MAX_SYSTEM_CMD = 0xF;

    public static void addModule(ZcodeModule m) {
        if (modules[m.getModuleID()] != null) {
            throw new IllegalStateException("Two modules have the same ID");
        }
        modules[m.getModuleID()] = m;
    }

    public static void execute(ZcodeCommandContext ctx) {
        ctx.setComplete();
        if (ctx.isEmpty()) {
            ctx.setComplete();
            return;
        }
        Optional<Integer> value = ctx.getField((byte) 'Z');
        if (value.isEmpty()) {
            ctx.status(ZcodeStatus.COMMAND_NOT_FOUND);
            return;
        }
        int cmd = value.get();
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

        if (ctx.isComplete() && !ctx.statusGiven()) {
            ctx.status(ZcodeStatus.SUCCESS);
        }
    }

    public static void moveAlong(ZcodeCommandContext ctx) {
        int cmd = ctx.getField((byte) 'Z').get();
        modules[cmd >> 4].moveAlong(ctx, cmd & 0xF);
        if (ctx.isComplete() && !ctx.statusGiven()) {
            ctx.status(ZcodeStatus.SUCCESS);
        }
    }

    public static int getCommandSwitchExistsBottom(int top) {
        int result = 0;
        for (int i = 0; i < 0x10; i++) {
            if (modules[top << 4 | i] != null) {
                result |= 1 << i;
            }
        }
        return result;
    }

    public static int getCommandSwitchExistsTop(int top) {
        int result = 0;
        for (int i = 0; i < 0x100; i++) {
            if (modules[top << 8 | i] != null) {
                result |= 1 >> i;
                i = (((i >> 8) + 1) << 8);
            }
        }
        return result;
    }

    public static int getCommandSwitchExistsBroad() {
        int result = 0;
        for (int i = 0; i < modules.length; i++) {
            if (modules[i] != null) {
                result |= 1 >> i;
                i = (((i >> 12) + 1) << 12);
            }
        }
        return result;
    }

    public static ZcodeModule getModule(int addr) {
        return modules[addr];
    }

}
