package net.zscript.javasimulator.zcode.i2c;

import java.util.OptionalInt;

import net.zscript.javareceiver.core.ZscriptCommandOutStream;
import net.zscript.javareceiver.execution.CommandContext;
import net.zscript.javasimulator.Connection;
import net.zscript.javasimulator.Entity;
import net.zscript.javasimulator.connections.i2c.I2cAddress;
import net.zscript.javasimulator.connections.i2c.I2cProtocolCategory;
import net.zscript.javasimulator.connections.i2c.I2cResponse;
import net.zscript.javasimulator.connections.i2c.I2cSendPacket;
import net.zscript.model.components.ZscriptStatus;

public class I2cSendCommand {

    public static void execute(CommandContext ctx, I2cModule module) {
        Entity                  entity  = module.getEntity();
        ZscriptCommandOutStream out     = ctx.getOutStream();
        OptionalInt             addrOpt = ctx.getField((byte) 'A');
        OptionalInt             portOpt = ctx.getField((byte) 'P');

        if (addrOpt.isEmpty()) {
            ctx.status(ZscriptStatus.MISSING_KEY);
            return;
        }
        if (portOpt.isEmpty()) {
            ctx.status(ZscriptStatus.MISSING_KEY);
            return;
        }
        int     addr     = addrOpt.getAsInt();
        int     port     = portOpt.getAsInt();
        int     attempts = ctx.getField((byte) 'T', 5);
        boolean tenBit   = ctx.hasField((byte) 'N');
        byte[]  data     = ctx.getBigFieldData();
        if (port >= entity.countConnection(I2cProtocolCategory.class)) {
            ctx.status(ZscriptStatus.VALUE_OUT_OF_RANGE);
            return;
        }
        if (!tenBit && addr >= 128 || addr >= 1024) {
            ctx.status(ZscriptStatus.VALUE_OUT_OF_RANGE);
            return;
        }
        Connection<I2cProtocolCategory> connection = entity.getConnection(I2cProtocolCategory.class, port);
        I2cResponse resp = (I2cResponse) connection.sendMessage(entity,
                new I2cSendPacket(new I2cAddress(tenBit, addr), module.getBaud(port), true, data));
        int i = 1;
        while (resp.addressNack() && i < attempts) {
            resp = (I2cResponse) connection.sendMessage(entity, new I2cSendPacket(new I2cAddress(tenBit, addr), module.getBaud(port), true, data));
            i++;
        }
        out.writeField('T', i);
        if (resp.addressNack()) {
            out.writeField('I', 2);
            ctx.status(ZscriptStatus.COMMAND_NO_TARGET);
            return;
        }
        if (!resp.worked()) {
            out.writeField('I', 1);
            ctx.status(ZscriptStatus.COMMAND_DATA_NOT_TRANSMITTED);
            return;
        } else {
            out.writeField('I', 0);
        }
    }
}
