package net.zscript.javareceiver.modules.core;

import java.nio.ByteBuffer;
import java.util.Iterator;
import java.util.UUID;

import net.zscript.javareceiver.core.ZscriptStatus;
import net.zscript.javareceiver.execution.CommandContext;

public class ZscriptGuidCommand {
    private UUID uuid = null;

    public void fetch(CommandContext ctx) {
        if (uuid == null) {
            ctx.status(ZscriptStatus.COMMAND_FAIL);
        } else {
            ByteBuffer bb = ByteBuffer.wrap(new byte[16]);
            bb.putLong(uuid.getMostSignificantBits());
            bb.putLong(uuid.getLeastSignificantBits());
            ctx.getOutStream().writeBig(bb.array());

        }
    }

    public void set(CommandContext ctx) {
        if (ctx.getBigFieldSize() != 16) {
            ctx.status(ZscriptStatus.VALUE_OUT_OF_RANGE);
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
