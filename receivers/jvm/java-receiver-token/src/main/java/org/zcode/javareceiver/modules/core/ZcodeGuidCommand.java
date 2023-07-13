package org.zcode.javareceiver.modules.core;

import java.nio.ByteBuffer;
import java.util.Iterator;
import java.util.UUID;

import org.zcode.javareceiver.core.ZcodeStatus;
import org.zcode.javareceiver.execution.ZcodeCommandContext;

public class ZcodeGuidCommand {
    private static UUID uuid = null;

    public static void fetch(ZcodeCommandContext ctx) {
        if (uuid == null) {
            ctx.status(ZcodeStatus.COMMAND_FAIL);
        } else {
            ByteBuffer bb = ByteBuffer.wrap(new byte[16]);
            bb.putLong(uuid.getMostSignificantBits());
            bb.putLong(uuid.getLeastSignificantBits());
            ctx.getOutStream().writeBig(bb.array());

        }
    }

    public static void set(ZcodeCommandContext ctx) {
        if (ctx.getBigFieldSize() != 16) {
            ctx.status(ZcodeStatus.VALUE_OUT_OF_RANGE);
        } else {
            byte[] data = new byte[16];
            int    i    = 0;
            for (Iterator<Byte> iterator = ctx.getBigField(); iterator.hasNext();) {
                byte b = iterator.next();
                data[i++] = b;
            }
            ByteBuffer bb = ByteBuffer.wrap(data);
            uuid = new UUID(bb.getLong(), bb.getLong());
        }
    }

}
