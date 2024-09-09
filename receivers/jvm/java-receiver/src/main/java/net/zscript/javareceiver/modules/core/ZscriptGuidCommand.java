package net.zscript.javareceiver.modules.core;

import java.nio.ByteBuffer;
import java.util.UUID;

import net.zscript.javareceiver.execution.CommandContext;
import net.zscript.model.components.ZscriptStatus;

public class ZscriptGuidCommand {
    private static final int  GUID_LENGTH = 16;
    private              UUID uuid        = null;

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
        if (ctx.getBigFieldSize() != GUID_LENGTH) {
            ctx.status(ZscriptStatus.VALUE_OUT_OF_RANGE);
        } else {
            ByteBuffer bb = ByteBuffer.wrap(ctx.getBigFieldData());
            uuid = new UUID(bb.getLong(), bb.getLong());
        }
    }
}
