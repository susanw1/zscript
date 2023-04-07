package org.zcode.javaclient.parsing;

import java.util.HashMap;
import java.util.Map;

public class Params {
    private Map<Byte, Integer> params;

    public Params(Map<Byte, Integer> params) {
        this.params = new HashMap<>(params);
    }

    public boolean has(byte symbol) {
        return params.containsKey(symbol);
    }

    public int get(byte f, int def) {
        return params.getOrDefault(f, def);
    }

    public long get(byte symbol) {
        Integer value = params.get(symbol);
        return value == null ? -1L : value.longValue();
    }

    int getCount() {
        return params.size();
    }

    public static ParamsBuilder builder() {
        return new ParamsBuilder();
    }

    public static class ParamsBuilder {
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

        public Params build() {
            return new Params(map);
        }

        public void reset() {
            map.clear();
            nibblePos = 0;
        }
    }
}
