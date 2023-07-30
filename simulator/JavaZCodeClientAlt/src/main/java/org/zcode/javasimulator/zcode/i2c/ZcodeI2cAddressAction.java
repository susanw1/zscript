package org.zcode.javasimulator.zcode.i2c;

import java.util.Iterator;
import java.util.Optional;

import org.zcode.javareceiver.core.ZcodeStatus;
import org.zcode.javareceiver.execution.ZcodeAddressingContext;
import org.zcode.javareceiver.tokenizer.OptIterator;
import org.zcode.javasimulator.Connection;
import org.zcode.javasimulator.Entity;
import org.zcode.javasimulator.connections.i2c.I2cAddress;
import org.zcode.javasimulator.connections.i2c.I2cProtocolCategory;
import org.zcode.javasimulator.connections.i2c.I2cResponse;
import org.zcode.javasimulator.connections.i2c.I2cSendPacket;

public class ZcodeI2cAddressAction {
    public static void execute(ZcodeAddressingContext ctx, I2cModule module) {
        Entity entity = module.getEntity();
        OptIterator<Integer> it = ctx.getAddressSegments();
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

        byte[] data = new byte[ctx.getAddressedDataSize()];
        int i = 0;
        for (Iterator<Byte> iterator = ctx.getAddressedData(); iterator.hasNext();) {
            data[i++] = iterator.next();
        }

        Connection<I2cProtocolCategory> connection = entity.getConnection(I2cProtocolCategory.class, port);
        I2cResponse resp = (I2cResponse) connection.sendMessage(entity, new I2cSendPacket(new I2cAddress(tenBit, addr), module.getBaud(port), false, data));
        if (!resp.worked()) {
            ctx.status(ZcodeStatus.ADDRESS_NOT_FOUND);
            return;
        }
    }

}
