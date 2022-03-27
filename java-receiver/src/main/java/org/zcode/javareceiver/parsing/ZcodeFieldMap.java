package org.zcode.javareceiver.parsing;

import org.zcode.javareceiver.Zchars;
import org.zcode.javareceiver.ZcodeOutStream;
import org.zcode.javareceiver.ZcodeParameters;

public class ZcodeFieldMap {
    private final ZcodeParameters params;
    private final char[]          fields;
    private final byte[]          values;
    private int                   size              = 0;
    private char                  lastSearchedField = 0;
    private byte                  lastFoundValue    = 0;

    public ZcodeFieldMap(final ZcodeParameters params) {
        this.params = params;
        this.fields = new char[params.fieldNum];
        this.values = new byte[params.fieldNum];
    }

    public boolean add(final char f, final byte v) {
        if (size == params.fieldNum) {
            return false;
        }
        fields[size] = f;
        values[size++] = v;
        lastSearchedField = f;
        lastFoundValue = v;
        return true;
    }

    public boolean has(final char f) {
        if (params.cacheFieldResults && f == lastSearchedField) {
            return true;
        }
        for (int i = 0; i < size; i++) {
            if (fields[i] == f) {
                if (params.cacheFieldResults) {
                    lastSearchedField = f;
                    lastFoundValue = values[i];
                }
                return true;
            }
        }
        return false;
    }

    public boolean has(final char f, final int fieldSectionNum) {
        int j = 0;
        for (int i = 0; i < size; i++) {
            if (fields[i] == f) {
                if (j++ == fieldSectionNum) {
                    return true;
                }
            }
        }
        return false;
    }

    public byte get(final char f, final byte def) {
        if (params.cacheFieldResults && f == lastSearchedField) {
            return lastFoundValue;
        }
        for (int i = 0; i < size; i++) {
            if (fields[i] == f) {
                if (params.cacheFieldResults) {
                    lastSearchedField = f;
                    lastFoundValue = values[i];
                }
                return values[i];
            }
        }
        return def;
    }

    public byte get(final char f, final int fieldSectionNum, final byte def) {
        int j = 0;
        for (int i = 0; i < size; i++) {
            if (fields[i] == f) {
                if (j++ == fieldSectionNum) {
                    return values[i];
                }
            }
        }
        return def;
    }

    public int countFieldSections(final char f) {
        int j = 0;
        for (int i = 0; i < size; i++) {
            if (fields[i] == f) {
                j++;
            }
        }
        return j;
    }

    public int getFieldCount() {
        return size;
    }

    public void reset() {
        size = 0;
        if (params.cacheFieldResults) {
            lastSearchedField = 0;
            lastFoundValue = 0;
        }
    }

    public void copyFieldTo(final ZcodeOutStream out, final char c) {
        boolean hasSeenBefore = false;
        for (byte i = 0; i < size; i++) {
            if (fields[i] == c) {
                if (!hasSeenBefore) {
                    out.writeField(c, values[i]);
                    hasSeenBefore = true;
                } else {
                    out.continueField(values[i]);
                }
            } else if (hasSeenBefore) {
                break;
            }
        }

    }

    public void copyTo(final ZcodeOutStream out) {
        char last = 0;
        for (byte i = 0; i < size; i++) {
            if (fields[i] != Zchars.ECHO_PARAM.ch && fields[i] != Zchars.CMD_PARAM.ch) {
                if (last == fields[i]) {
                    out.continueField(values[i]);
                } else {
                    out.writeField(fields[i], values[i]);
                    last = fields[i];
                }
            }
        }
    }
}
