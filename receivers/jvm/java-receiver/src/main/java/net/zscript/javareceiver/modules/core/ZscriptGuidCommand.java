package net.zscript.javareceiver.modules.core;

import java.nio.ByteBuffer;
import java.util.Optional;
import java.util.UUID;

import net.zscript.javareceiver.execution.CommandContext;
import net.zscript.model.components.ZscriptStatus;
import net.zscript.util.ByteString;

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
            ctx.getOutStream().writeFieldHex('R', bb.array());
        }
    }

    public void set(CommandContext ctx) {
        final Optional<ByteString> guid = ctx.getFieldAsByteString('G');
        if (guid.isEmpty()) {
            ctx.status(ZscriptStatus.MISSING_KEY);
        }
        if (guid.get().size() != GUID_LENGTH) {
            ctx.status(ZscriptStatus.VALUE_OUT_OF_RANGE);
        }
        ByteBuffer bb = ByteBuffer.wrap(guid.get().toByteArray());
        uuid = new UUID(bb.getLong(), bb.getLong());
    }
}
