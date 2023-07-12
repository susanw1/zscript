package org.zcode.javareceiver.core;

public interface ZcodeOutStream extends ZcodeCommandOutStream {

    void open();

    void close();

    boolean isOpen();

    void endSequence();

    void writeOrElse();

    void writeAndThen();

    void writeOpenParen();

    void writeCloseParen();
}
