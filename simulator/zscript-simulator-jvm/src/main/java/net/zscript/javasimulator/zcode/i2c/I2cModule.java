package net.zscript.javasimulator.zcode.i2c;

import java.util.ArrayList;
import java.util.List;

import net.zscript.javareceiver.core.OutStream;
import net.zscript.javareceiver.core.ZscriptStatus;
import net.zscript.javareceiver.execution.AddressingContext;
import net.zscript.javareceiver.execution.CommandContext;
import net.zscript.javareceiver.execution.NotificationContext;
import net.zscript.javareceiver.modules.ZscriptModule;
import net.zscript.javareceiver.notifications.ZscriptNotificationSource;
import net.zscript.javasimulator.Entity;
import net.zscript.javasimulator.connections.i2c.I2cProtocolCategory;

public class I2cModule implements ZscriptModule {
    private final Entity                 entity;
    private final I2cNotificationHandler notificationHandler = new I2cNotificationHandler();
    private final List<Long>             bauds               = new ArrayList<>();

    public I2cModule(Entity entity) {
        this.entity = entity;
    }

    public ZscriptNotificationSource getNotificationSource() {
        return notificationHandler.getNotificationSource();
    }

    public I2cNotificationHandler getNotificationHandler() {
        return notificationHandler;
    }

    @Override
    public int getModuleID() {
        return 5;
    }

    @Override
    public void execute(CommandContext ctx, int command) {
        switch (command) {
        case 0:
            I2cCapabilitiesCommand.execute(ctx, entity);
            break;
        case 1:
            I2cSetupCommand.execute(ctx, this);
            break;
        case 2:
            I2cSendCommand.execute(ctx, this);
            break;
        case 3:
            I2cReadCommand.execute(ctx, this);
            break;

        default:
            ctx.status(ZscriptStatus.COMMAND_NOT_FOUND);
            break;
        }
    }

    @Override
    public void address(AddressingContext ctx) {
        I2cAddressAction.execute(ctx, this);
    }

   
    public void notification(NotificationContext ctx, boolean moveAlong) {
        notificationHandler.notification(entity, ctx);
    }

    public Entity getEntity() {
        return entity;
    }

    public long getBaud(int index) {
        if (index >= bauds.size()) {
            return 100_000;
        }
        return bauds.get(index);
    }

    public void setBaud(int index, long baud) {
        while (bauds.size() <= index) {
            bauds.add((long) 100_000);
        }
        bauds.set(index, baud);
    }

    public void setBauds(long baud) {
        bauds.replaceAll(old -> baud);
        while (bauds.size() <= entity.countConnection(I2cProtocolCategory.class)) {
            bauds.add(baud);
        }
    }
}
