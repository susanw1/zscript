package net.zscript.zscript_acceptance_tests.acceptancetest_asserts;

import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.List;

public class AcceptanceTestBigField {
    private final List<Byte> field = new ArrayList<>();
    private boolean isString = false;

    public boolean isString() {
        return isString;
    }

    public void setIsString(boolean isString) {
        this.isString = isString;
    }

    public void addByte(byte b) {
        field.add(b);
    }

    public List<Byte> getField() {
        return field;
    }

    public String asString() {
        if (field.isEmpty()) {
            return "";
        }
        if (isString) {
            byte[] data = new byte[field.size()];
            int pos = 0;
            for (byte b : field) {
                data[pos] = b;
                pos++;
            }
            return "\"" + StandardCharsets.UTF_8.decode(ByteBuffer.wrap(data)).toString() + "\"";
        } else {
            String s = "+";
            for (byte b : field) {
                String hs = Integer.toHexString(Byte.toUnsignedInt(b));
                if (hs.length() == 1) {
                    s += '0' + Integer.toHexString(Byte.toUnsignedInt(b));
                } else {
                    s += Integer.toHexString(Byte.toUnsignedInt(b));
                }
            }
            return s;
        }
    }
}
