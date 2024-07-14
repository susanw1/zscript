package net.zscript.javaclient.commandPaths;

import net.zscript.model.components.Zchars;
import net.zscript.util.ByteString;

public class BigField implements ByteString.ByteAppendable {
    private final byte[]  data;
    private final boolean isString;

    public BigField(byte[] data, boolean isString) {
        this.data = data;
        this.isString = isString;
    }

    /**
     * Returns a copy of the enclosed data.
     *
     * @return copy of the data
     */
    public byte[] getData() {
        return data.clone();
    }

    public boolean isString() {
        return isString;
    }

    public int getDataLength() {
        return data.length;
    }

    @Override
    public void appendTo(ByteString.ByteStringBuilder builder) {
        if (isString) {
            builder.appendByte(Zchars.Z_BIGFIELD_QUOTED);
            for (byte b : data) {
                if (Zchars.mustStringEscape(b)) {
                    builder.appendByte(Zchars.Z_STRING_ESCAPE).appendHexPair(b);
                } else {
                    builder.appendByte(b);
                }
            }
            builder.appendByte(Zchars.Z_BIGFIELD_QUOTED);
        } else {
            builder.appendByte(Zchars.Z_BIGFIELD_HEX);
            for (byte b : data) {
                builder.appendHexPair(b);
            }
        }
    }
}
