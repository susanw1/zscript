package org.zcode.javasimulator.zcode.i2c;

import java.util.ArrayList;
import java.util.List;

import org.zcode.javareceiver.core.ZcodeOutStream;
import org.zcode.javareceiver.core.ZcodeStatus;
import org.zcode.javareceiver.execution.ZcodeAddressingContext;
import org.zcode.javareceiver.execution.ZcodeCommandContext;
import org.zcode.javareceiver.modules.ZcodeModule;
import org.zcode.javareceiver.notifications.ZcodeNotificationSource;
import org.zcode.javasimulator.Entity;
import org.zcode.javasimulator.connections.i2c.I2cProtocolCategory;

public class I2cModule implements ZcodeModule {
    private final Entity entity;
    private final I2cNotificationHandler notificationHandler = new I2cNotificationHandler();
    private List<Long> bauds = new ArrayList<>();

    public I2cModule(Entity entity) {
        this.entity = entity;
    }

    public ZcodeNotificationSource getNotificationSource() {
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
    public void execute(ZcodeCommandContext ctx, int command) {
        switch (command) {
        case 0:
            ZcodeI2cCapabilitiesCommand.execute(ctx, entity);
            break;
        case 1:
            ZcodeI2cSetupCommand.execute(ctx, this);
            break;
        case 2:
            ZcodeI2cSendCommand.execute(ctx, this);
            break;
        case 3:
            ZcodeI2cReadCommand.execute(ctx, this);
            break;

        default:
            ctx.status(ZcodeStatus.COMMAND_NOT_FOUND);
            break;
        }
    }

    @Override
    public void address(ZcodeAddressingContext ctx) {
        ZcodeI2cAddressAction.execute(ctx, this);
    }

    @Override
    public boolean notification(ZcodeOutStream out, int i, boolean isAddressed) {
        return notificationHandler.notification(entity, out, i, isAddressed);
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
