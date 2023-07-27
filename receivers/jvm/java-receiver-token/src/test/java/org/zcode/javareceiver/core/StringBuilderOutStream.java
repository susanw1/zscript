package org.zcode.javareceiver.core;

/**
 * Utility class for helping tests: ZcodeAbstractOutStream implementation that collects bytes
 */
public class StringBuilderOutStream extends ZcodeAbstractOutStream {
    StringBuilder str  = new StringBuilder();
    boolean       open = false;

    @Override
    public void open() {
        open = true;
    }

    @Override
    public void close() {
        open = false;
    }

    @Override
    public boolean isOpen() {
        return open;
    }

    @Override
    public void writeByte(byte b) {
        str.append((char) b);
    }

    public String getString() {
        return str.toString();
    }

    public String getStringAndReset() {
        final String string = str.toString();
        str.setLength(0);
        return string;
    }

    @Override
    public String toString() {
        return str.toString();
    }
}