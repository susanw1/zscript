package org.zcode.javareceiver.modules;

import java.util.Optional;

import org.zcode.javareceiver.core.ZcodeStatus;
import org.zcode.javareceiver.execution.ZcodeCommandView;

public class ZcodeCommandFinder {
    private static final ZcodeModule[] modules = new ZcodeModule[0x1000];

    private static final int MAX_SYSTEM_CMD = 0xF;

    public static void addModule(ZcodeModule m) {
        if (modules[m.getModuleID()] != null) {
            throw new IllegalStateException("Two modules have the same ID");
        }
        modules[m.getModuleID()] = m;
    }

    public static void execute(ZcodeCommandView view) {
        view.setComplete();
        if (view.isEmpty()) {
            view.setComplete();
            view.getOutStream().silentSucceed();
            return;
        }
        Optional<Integer> value = view.getField((byte) 'Z');
        if (value.isEmpty()) {
            view.status(ZcodeStatus.COMMAND_FORMAT_ERROR);
            return;
        }
        int cmd = value.get();
        if ((cmd >> 4) >= 0x1000) {
            view.status(ZcodeStatus.COMMAND_FORMAT_ERROR);
            return;
        }
        if (cmd > MAX_SYSTEM_CMD && !view.isActivated()) {
            view.status(ZcodeStatus.NOT_ACTIVATED);
            return;
        }
        if (modules[cmd >> 4] == null) {
            view.status(ZcodeStatus.COMMAND_FORMAT_ERROR);
            return;
        }
        modules[cmd >> 4].execute(view, cmd & 0xF);
        if (view.isComplete() && !view.statusGiven()) {
            view.status(ZcodeStatus.SUCCESS);
        }
    }

    public static void moveAlong(ZcodeCommandView view) {

        int cmd = view.getField((byte) 'Z').get();
        modules[cmd >> 4].moveAlong(view, cmd & 0xF);
        if (view.isComplete() && !view.statusGiven()) {
            view.status(ZcodeStatus.SUCCESS);
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
                result |= i >> 8;
                i = (((i >> 8) + 1) << 8);
            }
        }
        return result;
    }

    public static int getCommandSwitchExistsBroad() {
        int result = 0;
        for (int i = 0; i < modules.length; i++) {
            if (modules[i] != null) {
                result |= i >> 12;
                i = (((i >> 12) + 1) << 12);
            }
        }
        return result;
    }

}
