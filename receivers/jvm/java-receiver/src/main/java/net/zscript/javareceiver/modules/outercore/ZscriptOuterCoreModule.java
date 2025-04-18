package net.zscript.javareceiver.modules.outercore;

import net.zscript.javareceiver.core.LockSet;
import net.zscript.javareceiver.execution.CommandContext;
import net.zscript.javareceiver.execution.NotificationContext;
import net.zscript.javareceiver.modules.ZscriptModule;
import net.zscript.javareceiver.modules.core.ZscriptCoreModule;
import net.zscript.javareceiver.notifications.ZscriptNotificationSource;
import net.zscript.model.components.ZscriptStatus;

public class ZscriptOuterCoreModule implements ZscriptModule {
    private final ZscriptNotificationSource source = new ZscriptNotificationSource();

    public ZscriptOuterCoreModule() {
        source.setNotification(LockSet.allLocked(), 0x10);
    }

    public ZscriptNotificationSource getNotificationSource() {
        return source;
    }

    public static int getCommands() {
        return 0x0011;
    }

    @Override
    public int getModuleId() {
        return 1;
    }

    @Override
    public void execute(CommandContext ctx, int command) {
        switch (command) {
        case 0x0:
            ZscriptExtendedCapabilitiesCommand.execute(ctx);
            break;
        case 0x4:
            ((ZscriptCoreModule) ctx.getZscript().getModuleRegistry().getModule(0)).getGuidCmd().set(ctx);
            break;
        case 0x8:
            ZscriptChannelSetupCommand.execute(ctx);
            break;
        default:
            ctx.status(ZscriptStatus.COMMAND_NOT_FOUND);
        }
    }

    @Override
    public void notification(NotificationContext ctx, boolean moveAlong) {
        ctx.getOutStream().asCommandOutStream().writeFieldQuoted('M', "System has reset");
    }

}
