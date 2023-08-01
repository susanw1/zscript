package net.zscript.javareceiver.parsing;

import net.zscript.javareceiver.ZcodeOutStream;
import net.zscript.javareceiver.ZcodeParameters;

public class ZcodeBigField {
    private byte[]  data;
    private int     length;
    private boolean isString;

    public ZcodeBigField(final ZcodeParameters p, final boolean isHuge) {
        if (isHuge) {
            data = new byte[p.hugeFieldLength];
        } else {
            data = new byte[p.bigFieldStandardLength];
        }
        length = 0;
    }

    public boolean addByteToBigField(final byte b) {
        if (length >= data.length) {
            return false;
        }
        data[length++] = b;
        return true;
    }

    public byte[] getData() {
        return data;
    }

    public boolean isInUse() {
        return length != 0;
    }

    public int getLength() {
        return length;
    }

    public void setIsString(final boolean isString) {
        this.isString = isString;
    }

    public void reset() {
        length = 0;
    }

    public void copyTo(final ZcodeBigField target) {
        target.length = length;
        for (int i = 0; i < length; i++) {
            target.data[i] = data[i];
        }
    }

    public void copyTo(final ZcodeOutStream out) {
        if (length != 0) {
            if (isString) {
                out.writeBigStringField(data, length);
            } else {
                out.writeBigHexField(data, length);
            }
        }
    }

    public boolean isString() {
        return isString;
    }

}
