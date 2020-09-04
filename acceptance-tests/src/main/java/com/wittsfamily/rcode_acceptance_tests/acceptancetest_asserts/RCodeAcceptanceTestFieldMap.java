package com.wittsfamily.rcode_acceptance_tests.acceptancetest_asserts;

import java.util.HashMap;
import java.util.Map;
import java.util.Map.Entry;

public class RCodeAcceptanceTestFieldMap {
    private final Map<Character, byte[]> fields = new HashMap<>();

    public void addField(char c, byte b) {
        if (fields.containsKey(c)) {
            byte[] old = fields.get(c);
            byte[] withNew = new byte[old.length + 1];
            System.arraycopy(old, 0, withNew, 0, old.length);
            withNew[withNew.length - 1] = b;
            fields.put(c, withNew);
        } else {
            fields.put(c, new byte[] { b });
        }
    }

    public void addField(char c, byte[] val) {
        fields.put(c, val);
    }

    public int getFieldLength(char c) {
        if (!fields.containsKey(c)) {
            return 0;
        }
        return fields.get(c).length;
    }

    public byte[] getField(char c) {
        return fields.get(c);
    }

    public boolean hasField(char c) {
        return fields.containsKey(c);
    }

    public String asString() {
        String s = "";
        for (Entry<Character, byte[]> e : fields.entrySet()) {
            s += e.getKey();
            if (e.getValue().length == 1) {
                String hs = Integer.toHexString(Byte.toUnsignedInt(e.getValue()[0]));
                if (hs.equals("0")) {
                } else {
                    s += Integer.toHexString(Byte.toUnsignedInt(e.getValue()[0]));
                }
            } else {
                for (byte b : e.getValue()) {
                    String hs = Integer.toHexString(Byte.toUnsignedInt(b));
                    if (hs.length() == 1) {
                        s += '0' + Integer.toHexString(Byte.toUnsignedInt(b));
                    } else {
                        s += Integer.toHexString(Byte.toUnsignedInt(b));
                    }
                }
            }
        }
        return s;
    }

    public String asString(char f) {
        String s = "";
        byte[] data = fields.get(f);
        s += f;
        if (data.length == 1) {
            String hs = Integer.toHexString(Byte.toUnsignedInt(data[0]));
            if (hs.equals("0")) {
            } else {
                s += Integer.toHexString(Byte.toUnsignedInt(data[0]));
            }
        } else {
            for (byte b : data) {
                String hs = Integer.toHexString(Byte.toUnsignedInt(b));
                if (hs.length() == 1) {
                    s += '0' + Integer.toHexString(Byte.toUnsignedInt(b));
                } else {
                    s += Integer.toHexString(Byte.toUnsignedInt(b));
                }
            }
        }
        return s;
    }
}
