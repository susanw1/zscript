package org.zcode.javareceiver.core;

public interface ZcodeOutStream {

    void open();

    void endSequence();

    void close();

    boolean isOpen();

    void silentSucceed();

    void writeString(String string);

    void startField(byte key);

    void continueField(byte next);

    void writeField(byte field, int value);

    void writeField(char field, int value);

    void writeByte(byte c);

    void writeOrElse();

    void writeAndThen();

}
