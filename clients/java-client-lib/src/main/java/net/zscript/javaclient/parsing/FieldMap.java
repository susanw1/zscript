package net.zscript.javaclient.parsing;

import java.util.HashMap;
import java.util.Map;

public class FieldMap {
    private Map<Byte, Integer> map = new HashMap<>();
    private int                nibblePos;
    private byte               lastField;

    boolean add16(byte f, int v) {
        nibblePos = 0;
        lastField = f;
        return map.putIfAbsent(f, v) == null;
    }

    boolean addBlank(byte f) {
        return add16(f, 0);
    }

    boolean add4(byte nibble) {
        if (nibblePos >= 4) {
            return false;
        }
        map.compute(lastField, (key, oldValue) -> (oldValue << 4) | nibble);
        nibblePos++;
        return true;
    }

    boolean add8(byte octet) {
        if (nibblePos >= 3) {
            return false;
        }
        map.compute(lastField, (key, oldValue) -> (oldValue << 8) | octet);
        nibblePos += 2;
        return true;
    }

    boolean has(byte f) {
        return map.containsKey(f);
    }

    int get(byte f, int def) {
        return map.getOrDefault(f, def);
    }

    long get(byte f) {
        Integer value = map.get(f);
        return value == null ? -1L : value.longValue();
    }

    int getFieldCount() {
        return map.size();
    }

    void reset() {
        map.clear();
    }

//    void copyFieldTo(ZcodeOutStream<ZP> *out, byte c)  {
//            for (uint8_t i = 0; i < size; i++) {
//                if (fields[i] == c) {
//                    out->writeField16(c, values[i]);
//                }
//            }
//        }
//
//    void copyTo(ZcodeOutStream<ZP> *out)  {
//            for (uint8_t i = 0; i < size; i++) {
//                if (fields[i] != Zchars::ECHO_PARAM && fields[i] != Zchars::CMD_PARAM && fields[i] != Zchars::STATUS_RESP_PARAM) {
//                    out->writeField16(fields[i], values[i]);
//                }
//            }
//        }
}
