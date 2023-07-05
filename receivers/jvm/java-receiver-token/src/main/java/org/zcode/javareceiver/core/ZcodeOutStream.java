package org.zcode.javareceiver.core;

import org.zcode.javareceiver.execution.ZcodeField;

public interface ZcodeOutStream {

    void open();

    void close();

    boolean isOpen();

    void writeByte(byte c);

    void writeString(String string);

    void writeString(byte[] data);

    void writeBig(byte[] data);

    void writeField(byte field, int value);

    default void writeField(char field, int value) {
        writeField((byte) field, value);
    }

    void writeField(ZcodeField field);

    void endSequence();

    void writeOrElse();

    void writeAndThen();

    void writeOpenParen();

    void writeCloseParen();

}
