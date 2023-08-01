package net.zscript.javasimulator.zcode.i2c;

import java.util.Iterator;
import java.util.Optional;

import net.zscript.javareceiver.core.ZcodeStatus;
import net.zscript.javareceiver.execution.ZcodeAddressingContext;
import net.zscript.javareceiver.tokenizer.OptIterator;
import net.zscript.javasimulator.Connection;
import net.zscript.javasimulator.Entity;
import net.zscript.javasimulator.connections.i2c.I2cAddress;
import net.zscript.javasimulator.connections.i2c.I2cProtocolCategory;
import net.zscript.javasimulator.connections.i2c.I2cResponse;
import net.zscript.javasimulator.connections.i2c.I2cSendPacket;

public class ZcodeI2cAddressAction {
    public static void execute(ZcodeAddressingContext ctx, I2cModule module) {
        Entity               entity = module.getEntity();
        OptIterator<Integer> it     = ctx.getAddressSegments();
        if (!it.next().isPresent()) {
            ctx.status(ZcodeStatus.INTERNAL_ERROR);
            return;
        }
        Optional<Integer> oint = it.next();
        if (!oint.isPresent()) {
            ctx.status(ZcodeStatus.INTERNAL_ERROR);
            return;
        }

        int port = oint.get();
        oint = it.next();
        if (!oint.isPresent()) {
            ctx.status(ZcodeStatus.INTERNAL_ERROR);
            return;
        }
        boolean tenBit = (port & 0x8000) != 0;
        port &= ~0x8000;
        int addr = oint.get();
        if (port >= entity.countConnection(I2cProtocolCategory.class)) {
            ctx.status(ZcodeStatus.VALUE_OUT_OF_RANGE);
            return;
        }
        if (!tenBit && addr >= 128 || addr >= 1024) {
            ctx.status(ZcodeStatus.VALUE_OUT_OF_RANGE);
            return;
        }

        byte[] data = new byte[ctx.getAddressedDataSize() + 1];
        int    i    = 0;
        for (Iterator<Byte> iterator = ctx.getAddressedData(); iterator.hasNext();) {
            data[i++] = iterator.next();
        }
        data[i++] = '\n';
        Connection<I2cProtocolCategory> connection = entity.getConnection(I2cProtocolCategory.class, port);
        I2cResponse                     resp       = (I2cResponse) connection.sendMessage(entity,
                new I2cSendPacket(new I2cAddress(tenBit, addr), module.getBaud(port), true, data));
        if (!resp.worked()) {
            ctx.status(ZcodeStatus.ADDRESS_NOT_FOUND);
            return;
        }
    }

}
