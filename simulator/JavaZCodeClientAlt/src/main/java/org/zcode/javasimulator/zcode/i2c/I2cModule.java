package org.zcode.javasimulator.zcode.i2c;

import java.util.ArrayList;
import java.util.List;

import org.zcode.javareceiver.core.ZcodeStatus;
import org.zcode.javareceiver.execution.ZcodeAddressingContext;
import org.zcode.javareceiver.execution.ZcodeCommandContext;
import org.zcode.javareceiver.modules.ZcodeModule;
import org.zcode.javasimulator.Entity;
import org.zcode.javasimulator.connections.i2c.I2cProtocolCategory;

public class I2cModule implements ZcodeModule {
    private final Entity entity;
    private List<Long> bauds = new ArrayList<>();

    public I2cModule(Entity entity) {
        this.entity = entity;
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
    public void moveAlong(ZcodeCommandContext ctx, int command) {
        throw new IllegalStateException("No I2C commands are async");
    }

    @Override
    public void address(ZcodeAddressingContext ctx) {
        // TODO Auto-generated method stub

    }

    @Override
    public void addressMoveAlong(ZcodeAddressingContext ctx) {
        // TODO Auto-generated method stub

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
