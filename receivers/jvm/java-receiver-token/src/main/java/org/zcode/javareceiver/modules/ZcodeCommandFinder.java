package org.zcode.javareceiver.modules;

import java.util.Optional;

import org.zcode.javareceiver.core.ZcodeStatus;
import org.zcode.javareceiver.execution.ZcodeCommandEnviroment;
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
        ZcodeCommandEnviroment env   = new ZcodeCommandEnviroment(view);
        Optional<Integer>      value = env.getField((byte) 'Z');
        if (value.isEmpty()) {
            env.status(ZcodeStatus.COMMAND_FORMAT_ERROR);
            return;
        }
        int cmd = value.get();
        if ((cmd >> 4) >= 0x1000) {
            env.status(ZcodeStatus.COMMAND_FORMAT_ERROR);
            return;
        }
        if (cmd > MAX_SYSTEM_CMD && !view.isActivated()) {
            env.status(ZcodeStatus.NOT_ACTIVATED);
            return;
        }
        if (modules[cmd >> 4] == null) {
            env.status(ZcodeStatus.COMMAND_FORMAT_ERROR);
            return;
        }
        modules[cmd >> 4].execute(env, cmd & 0xF);
        if (view.isComplete() && !env.statusGiven()) {
            env.status(ZcodeStatus.SUCCESS);
        }
    }

    public static void moveAlong(ZcodeCommandView view) {
        ZcodeCommandEnviroment env = new ZcodeCommandEnviroment(view);

        int cmd = env.getField((byte) 'Z').get();
        modules[cmd >> 4].moveAlong(env, cmd & 0xF);
        if (view.isComplete() && !env.statusGiven()) {
            env.status(ZcodeStatus.SUCCESS);
        }
    }
}
