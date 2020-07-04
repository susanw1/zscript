package com.wittsfamily.rcode.parsing;

import com.wittsfamily.rcode.RCodeOutStream;
import com.wittsfamily.rcode.RCodeParameters;

public class RCodeBigField {
    private byte[] data;
    private int length;
    private final RCodeParameters params;
    private boolean isString;

    public RCodeBigField(RCodeParameters p, boolean isBigBig) {
        params = p;
        if (isBigBig) {
            data = new byte[p.bigBigFieldLength];
        } else {
            data = new byte[p.bigFieldStandardLength];
        }
        length = 0;
    }

    public boolean addByteToBigField(byte b) {
        if (length >= data.length) {
            return false;
        }
        data[length++] = b;
        return true;
    }

    public byte[] getData() {
        return data;
    }

    public int getLength() {
        return length;
    }

    public void setIsString(boolean isString) {
        this.isString = isString;
    }

    public void reset() {
        length = 0;
    }

    public void copyTo(RCodeBigField target) {
        target.length = length;
        for (int i = 0; i < length; i++) {
            target.data[i] = data[i];
        }
    }

    public void copyTo(RCodeOutStream out) {
        if (length != 0) {
            if (isString) {
                out.writeBigStringField(data, length);
            } else {
                out.writeBigHexField(data, length);
            }
        }
    }

}
