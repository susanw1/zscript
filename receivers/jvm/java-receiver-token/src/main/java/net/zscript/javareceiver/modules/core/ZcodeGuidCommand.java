package net.zscript.javareceiver.modules.core;

import java.nio.ByteBuffer;
import java.util.Iterator;
import java.util.UUID;

import net.zscript.javareceiver.core.ZcodeStatus;
import net.zscript.javareceiver.execution.ZcodeCommandContext;

public class ZcodeGuidCommand {
    private UUID uuid = null;

    public void fetch(ZcodeCommandContext ctx) {
        if (uuid == null) {
            ctx.status(ZcodeStatus.COMMAND_FAIL);
        } else {
            ByteBuffer bb = ByteBuffer.wrap(new byte[16]);
            bb.putLong(uuid.getMostSignificantBits());
            bb.putLong(uuid.getLeastSignificantBits());
            ctx.getOutStream().writeBig(bb.array());

        }
    }

    public void set(ZcodeCommandContext ctx) {
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
