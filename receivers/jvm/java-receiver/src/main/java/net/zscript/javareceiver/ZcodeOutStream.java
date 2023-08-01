package net.zscript.javareceiver;

import net.zscript.javareceiver.parsing.ZcodeCommandChannel;

public abstract class ZcodeOutStream {
    public Object  mostRecent;
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

    public abstract ZcodeOutStream markNotification();

    public abstract ZcodeOutStream markBroadcast();

    public abstract ZcodeOutStream writeStatus(ZcodeResponseStatus st);

    public abstract ZcodeOutStream writeField(char f, byte v);

    public abstract ZcodeOutStream continueField(byte v);

    public abstract ZcodeOutStream writeBigHexField(byte[] value, int length);

    public abstract ZcodeOutStream writeBigStringField(byte[] value, int length);

    public abstract ZcodeOutStream writeBigStringField(String s);

    public abstract ZcodeOutStream writeBytes(byte[] value, int length);

    public abstract ZcodeOutStream writeCommandSeparator();

    public abstract ZcodeOutStream writeCommandSequenceSeparator();

    public abstract ZcodeOutStream writeCommandSequenceErrorHandler();

    public abstract void openResponse(ZcodeCommandChannel target);

    public abstract void openNotification(ZcodeCommandChannel target);

    public abstract void openDebug(ZcodeCommandChannel target);

    public abstract boolean isOpen();

    public abstract void close();

}
