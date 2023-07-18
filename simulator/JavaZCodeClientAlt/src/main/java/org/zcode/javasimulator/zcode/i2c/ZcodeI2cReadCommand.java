package org.zcode.javasimulator.zcode.i2c;

import java.util.Optional;

import org.zcode.javareceiver.core.ZcodeCommandOutStream;
import org.zcode.javareceiver.core.ZcodeStatus;
import org.zcode.javareceiver.execution.ZcodeCommandContext;
import org.zcode.javasimulator.Connection;
import org.zcode.javasimulator.Entity;
import org.zcode.javasimulator.connections.i2c.I2cAddress;
import org.zcode.javasimulator.connections.i2c.I2cProtocolCategory;
import org.zcode.javasimulator.connections.i2c.I2cReceivePacket;
import org.zcode.javasimulator.connections.i2c.I2cReceiveResponse;
import org.zcode.javasimulator.connections.i2c.I2cResponse;

public class ZcodeI2cReadCommand {

    public static void execute(ZcodeCommandContext ctx, I2cModule module) {
        Entity entity = module.getEntity();
        ZcodeCommandOutStream out = ctx.getOutStream();
        Optional<Integer> addrOpt = ctx.getField((byte) 'A');
        Optional<Integer> portOpt = ctx.getField((byte) 'P');
        Optional<Integer> lengthOpt = ctx.getField((byte) 'L');

        if (addrOpt.isEmpty()) {
            ctx.status(ZcodeStatus.MISSING_KEY);
            return;
        }
        if (portOpt.isEmpty()) {
            ctx.status(ZcodeStatus.MISSING_KEY);
            return;
        }
        if (lengthOpt.isEmpty()) {
            ctx.status(ZcodeStatus.MISSING_KEY);
            return;
        }
        int addr = addrOpt.get();
        int port = portOpt.get();
        int length = lengthOpt.get();
        int attempts = ctx.getField((byte) 'T', 1);
        boolean tenBit = ctx.hasField((byte) 'N');
        if (port >= entity.countConnection(I2cProtocolCategory.class)) {
            ctx.status(ZcodeStatus.VALUE_OUT_OF_RANGE);
            return;
        }
        if (!tenBit && addr >= 128 || addr >= 1024) {
            ctx.status(ZcodeStatus.VALUE_OUT_OF_RANGE);
            return;
        }

        Connection<I2cProtocolCategory> connection = entity.getConnection(I2cProtocolCategory.class, port);
        I2cResponse resp = (I2cResponse) connection.sendMessage(entity, new I2cReceivePacket(new I2cAddress(tenBit, addr), module.getBaud(port), false, length));
        int i = 1;
        while (resp.addressNack() && i < attempts) {
            resp = (I2cResponse) connection.sendMessage(entity, new I2cReceivePacket(new I2cAddress(tenBit, addr), module.getBaud(port), false, length));
            i++;
        }
        out.writeField('T', i);
        if (resp.addressNack()) {
            out.writeField('I', 2);
            ctx.status(ZcodeStatus.COMMAND_NO_TARGET);
        } else {
            out.writeField('I', 0);
            I2cReceiveResponse recResp = (I2cReceiveResponse) resp;
            out.writeBig(recResp.getData());
        }
    }
}
