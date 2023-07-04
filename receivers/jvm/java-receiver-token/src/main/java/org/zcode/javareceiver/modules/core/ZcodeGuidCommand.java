package org.zcode.javareceiver.modules.core;

import java.nio.ByteBuffer;
import java.util.Iterator;
import java.util.UUID;

import org.zcode.javareceiver.core.ZcodeStatus;
import org.zcode.javareceiver.execution.ZcodeCommandView;

public class ZcodeGuidCommand {
    private static UUID uuid = null;

    public static void fetch(ZcodeCommandView view) {
        if (uuid == null) {
            view.status(ZcodeStatus.COMMAND_FAIL);
        } else {
            ByteBuffer bb = ByteBuffer.wrap(new byte[16]);
            bb.putLong(uuid.getMostSignificantBits());
            bb.putLong(uuid.getLeastSignificantBits());
            view.getOutStream().writeBig(bb.array());

        }
    }

    public static void set(ZcodeCommandView view) {
        if (view.getBigFieldSize() != 16) {
            view.status(ZcodeStatus.COMMAND_FORMAT_ERROR);
        } else {
            byte[] data = new byte[16];
            int    i    = 0;
            for (Iterator<Byte> iterator = view.getBigField(); iterator.hasNext();) {
                byte b = iterator.next();
                data[i++] = b;
            }
            ByteBuffer bb = ByteBuffer.wrap(data);
            uuid = new UUID(bb.getLong(), bb.getLong());
        }
    }

}
