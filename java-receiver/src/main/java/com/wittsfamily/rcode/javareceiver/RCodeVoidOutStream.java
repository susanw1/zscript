package com.wittsfamily.rcode.javareceiver;

import com.wittsfamily.rcode.javareceiver.parsing.RCodeCommandChannel;

public class RCodeVoidOutStream extends RCodeOutStream {

    @Override
    public RCodeOutStream markNotification() {
        return this;
    }

    @Override
    public RCodeOutStream markBroadcast() {
        return this;
    }

    @Override
    public RCodeOutStream writeStatus(RCodeResponseStatus st) {
        return this;
    }

    @Override
    public RCodeOutStream writeField(char f, byte v) {
        return this;
    }

    @Override
    public RCodeOutStream continueField(byte v) {
        return this;
    }

    @Override
    public RCodeOutStream writeBigHexField(byte[] value, int length) {
        return this;
    }

    @Override
    public RCodeOutStream writeBigStringField(byte[] value, int length) {
        return this;
    }

    @Override
    public RCodeOutStream writeBigStringField(String s) {
        return this;
    }

    @Override
    public RCodeOutStream writeBytes(byte[] value, int length) {
        return this;
    }

    @Override
    public RCodeOutStream writeCommandSeperator() {
        return this;
    }

    @Override
    public RCodeOutStream writeCommandSequenceSeperator() {
        return this;
    }

    @Override
    public void openResponse(RCodeCommandChannel target) {
    }

    @Override
    public void openNotification(RCodeCommandChannel target) {
    }

    @Override
    public void openDebug(RCodeCommandChannel target) {
    }

    @Override
    public boolean isOpen() {
        return false;
    }

    @Override
    public void close() {
    }

    @Override
    public boolean isLocked() {
        return false;
    }

    @Override
    public boolean lock() {
        return true;
    }
}
