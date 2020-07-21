package com.wittsfamily.rcode.executionspace;

import com.wittsfamily.rcode.RCodeNotificationManager;
import com.wittsfamily.rcode.RCodeOutStream;
import com.wittsfamily.rcode.RCodeParameters;
import com.wittsfamily.rcode.RCodeResponseStatus;
import com.wittsfamily.rcode.parsing.RCodeCommandChannel;

public class RCodeInterruptVectorOut extends RCodeOutStream {
    private final RCodeNotificationManager notificationManager;
    private final RCodeParameters params;
    private RCodeOutStream out = null;

    public RCodeInterruptVectorOut(RCodeNotificationManager notificationManager, RCodeParameters params) {
        this.notificationManager = notificationManager;
        this.params = params;
    }

    @Override
    public RCodeOutStream markNotification() {
        return out.markNotification();
    }

    @Override
    public RCodeOutStream markBroadcast() {
        return out.markBroadcast();
    }

    @Override
    public RCodeOutStream writeStatus(RCodeResponseStatus st) {
        return out.writeStatus(st);
    }

    @Override
    public RCodeOutStream writeField(char f, byte v) {
        return out.writeField(f, v);
    }

    @Override
    public RCodeOutStream continueField(byte v) {
        return out.continueField(v);
    }

    @Override
    public RCodeOutStream writeBigHexField(byte[] value, int length) {
        return out.writeBigHexField(value, length);
    }

    @Override
    public RCodeOutStream writeBigStringField(byte[] value, int length) {
        return out.writeBigStringField(value, length);
    }

    @Override
    public RCodeOutStream writeBigStringField(String s) {
        return out.writeBigStringField(s);
    }

    @Override
    public RCodeOutStream writeBytes(byte[] value, int length) {
        return out.writeBytes(value, length);
    }

    @Override
    public RCodeOutStream writeCommandSeperator() {
        return out.writeCommandSeperator();
    }

    @Override
    public RCodeOutStream writeCommandSequenceSeperator() {
        return out.writeCommandSequenceSeperator();
    }

    @Override
    public void openResponse(RCodeCommandChannel target) {
        RCodeInterruptVectorChannel channel = (RCodeInterruptVectorChannel) target;
        if (out.isOpen()) {
            out.close();
        }
        out.openNotification();
        out.markNotification();
        out.writeField('Z', (byte) 1);
        out.writeField('A', (byte) 1);
        out.writeField('T', channel.getInterrupt().getNotificationType());
        out.writeField('I', channel.getInterrupt().getNotificationBus());
        out.writeStatus(RCodeResponseStatus.OK);
        if (params.findInterruptSourceAddress && channel.getInterrupt().getSource().hasAddress()) {
            out.writeCommandSeperator();
            out.writeField('A', channel.getInterrupt().getFoundAddress());
            if (channel.getInterrupt().hasFindableAddress()) {
                out.writeStatus(RCodeResponseStatus.OK);
            } else {
                out.writeStatus(RCodeResponseStatus.CMD_FAIL);
            }
        }
        out.writeCommandSeperator();
        channel.getInterrupt().clear();
    }

    @Override
    public void openNotification() {
        throw new UnsupportedOperationException("Interrupt Vector Out streams are response only");
    }

    @Override
    public boolean isOpen() {
        return out.isOpen();
    }

    @Override
    public void close() {
        out.close();
    }

    @Override
    public boolean lock() {
        if (out.lock()) {
            out = notificationManager.getNotificationChannel().getOutStream();
            return true;
        } else {
            return false;
        }
    }

    @Override
    public boolean isLocked() {
        if (out == null) {
            out = notificationManager.getNotificationChannel().getOutStream();
        }
        return out.isLocked();
    }

    @Override
    public void unlock() {
        out.unlock();
    }
}
