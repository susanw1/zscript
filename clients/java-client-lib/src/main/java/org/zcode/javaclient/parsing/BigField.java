package org.zcode.javaclient.parsing;

import java.io.ByteArrayOutputStream;

public class BigField {
    private ByteArrayOutputStream big;
    private int                   pos;

    private byte    currentHexByte;
    private boolean inNibble;

    public BigField() {
        this.big = new ByteArrayOutputStream();
    }

    public byte[] getData() {
        return big.toByteArray();
    }

    public int getLength() {
        return pos;
    }

    public void reset() {
        pos = 0;
        currentHexByte = 0;
        inNibble = false;
    }

    public boolean isAtEnd() {
        return false;
    }

    public boolean addByte(byte c) {
        if (inNibble) {
            return false;
        }
        big.write(c & 0xFF);
        return true;
    }

    public boolean addNibble(byte nibble) {
        if (inNibble) {
            big.write((currentHexByte << 4 | nibble) & 0xFF);
            currentHexByte = 0;
        } else {
            currentHexByte = nibble;
        }
        inNibble = !inNibble;
        return true;
    }

    public boolean isInNibble() {
        return inNibble;
    }
}
