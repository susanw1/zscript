package org.zcode.javareceiver.modules.outerCore;

import org.zcode.javareceiver.core.ZcodeLockSet;
import org.zcode.javareceiver.core.ZcodeOutStream;
import org.zcode.javareceiver.core.ZcodeStatus;
import org.zcode.javareceiver.execution.ZcodeCommandContext;
import org.zcode.javareceiver.modules.ZcodeModule;
import org.zcode.javareceiver.modules.core.ZcodeCoreModule;
import org.zcode.javareceiver.notifications.ZcodeNotificationSource;

public class ZcodeOuterCoreModule implements ZcodeModule {
    private ZcodeNotificationSource source = new ZcodeNotificationSource();

    public ZcodeOuterCoreModule() {
        source.setNotification(ZcodeLockSet.allLocked(), 0x10);
    }

    public ZcodeNotificationSource getNotificationSource() {
        return source;
    }

    public static int getCommands() {
        return 0x0011;
    }

    @Override
    public int getModuleID() {
        return 1;
    }

    @Override
    public void execute(ZcodeCommandContext ctx, int command) {
        switch (command) {
        case 0x0:
            ZcodeExtendedCapabilitiesCommand.execute(ctx);
            break;
        case 0x4:
            ((ZcodeCoreModule) ctx.getZcode().getModuleRegistry().getModule(0)).getGuidCmd().set(ctx);
            break;
        case 0x8:
            ZcodeChannelSetupCommand.execute(ctx);
            break;
        default:
            ctx.status(ZcodeStatus.COMMAND_NOT_FOUND);
        }
    }

    @Override
    public boolean notification(ZcodeOutStream out, int i, boolean isAddressed) {
        out.writeString("System has reset");
        return true;
    }

}
