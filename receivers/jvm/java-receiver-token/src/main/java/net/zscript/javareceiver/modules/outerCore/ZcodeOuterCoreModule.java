package net.zscript.javareceiver.modules.outerCore;

import net.zscript.javareceiver.core.ZcodeLockSet;
import net.zscript.javareceiver.core.ZcodeOutStream;
import net.zscript.javareceiver.core.ZcodeStatus;
import net.zscript.javareceiver.execution.ZcodeCommandContext;
import net.zscript.javareceiver.modules.ZcodeModule;
import net.zscript.javareceiver.modules.core.ZcodeCoreModule;
import net.zscript.javareceiver.notifications.ZcodeNotificationSource;

public class ZcodeOuterCoreModule implements ZcodeModule {
    private final ZcodeNotificationSource source = new ZcodeNotificationSource();

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
        out.asCommandOutStream().writeQuotedString("System has reset");
        return true;
    }

}
