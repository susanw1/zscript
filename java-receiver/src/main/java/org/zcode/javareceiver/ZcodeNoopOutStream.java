package org.zcode.javareceiver;

import org.zcode.javareceiver.parsing.ZcodeCommandChannel;

public class ZcodeNoopOutStream extends ZcodeOutStream {

    @Override
    public ZcodeOutStream markNotification() {
        return this;
    }

    @Override
    public ZcodeOutStream markBroadcast() {
        return this;
    }

    @Override
    public ZcodeOutStream writeStatus(final ZcodeResponseStatus st) {
        return this;
    }

    @Override
    public ZcodeOutStream writeField(final char f, final byte v) {
        return this;
    }

    @Override
    public ZcodeOutStream continueField(final byte v) {
        return this;
    }

    @Override
    public ZcodeOutStream writeBigHexField(final byte[] value, final int length) {
        return this;
    }

    @Override
    public ZcodeOutStream writeBigStringField(final byte[] value, final int length) {
        return this;
    }

    @Override
    public ZcodeOutStream writeBigStringField(final String s) {
        return this;
    }

    @Override
    public ZcodeOutStream writeBytes(final byte[] value, final int length) {
        return this;
    }

    @Override
    public ZcodeOutStream writeCommandSeperator() {
        return this;
    }

    @Override
    public ZcodeOutStream writeCommandSequenceSeperator() {
        return this;
    }

    @Override
    public ZcodeOutStream writeCommandSequenceErrorHandler() {
        return this;
    }

    @Override
    public void openResponse(final ZcodeCommandChannel target) {
    }

    @Override
    public void openNotification(final ZcodeCommandChannel target) {
    }

    @Override
    public void openDebug(final ZcodeCommandChannel target) {
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
