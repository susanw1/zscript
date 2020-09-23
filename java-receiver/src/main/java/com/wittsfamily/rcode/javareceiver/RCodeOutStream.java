package com.wittsfamily.rcode.javareceiver;

import com.wittsfamily.rcode.javareceiver.parsing.RCodeCommandChannel;

public abstract class RCodeOutStream {
    public Object mostRecent;
    public boolean lock;

    public boolean lock() {
        if (!lock) {
            lock = true;
            return true;
        }
        return false;

    }

    public void unlock() {
        lock = false;
    }

    public boolean isLocked() {
        return lock;
    }

    public abstract RCodeOutStream markNotification();

    public abstract RCodeOutStream markBroadcast();

    public abstract RCodeOutStream writeStatus(RCodeResponseStatus st);

    public abstract RCodeOutStream writeField(char f, byte v);

    public abstract RCodeOutStream continueField(byte v);

    public abstract RCodeOutStream writeBigHexField(byte[] value, int length);

    public abstract RCodeOutStream writeBigStringField(byte[] value, int length);

    public abstract RCodeOutStream writeBigStringField(String s);

    public abstract RCodeOutStream writeBytes(byte[] value, int length);

    public abstract RCodeOutStream writeCommandSeperator();

    public abstract RCodeOutStream writeCommandSequenceSeperator();

    public abstract RCodeOutStream writeCommandSequenceErrorHandler();

    public abstract void openResponse(RCodeCommandChannel target);

    public abstract void openNotification(RCodeCommandChannel target);

    public abstract void openDebug(RCodeCommandChannel target);

    public abstract boolean isOpen();

    public abstract void close();

}
