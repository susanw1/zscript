package org.zcode.javareceiver.executionspace;

import org.zcode.javareceiver.Zchars;
import org.zcode.javareceiver.ZcodeNotificationManager;
import org.zcode.javareceiver.ZcodeOutStream;
import org.zcode.javareceiver.ZcodeParameters;
import org.zcode.javareceiver.ZcodeResponseStatus;
import org.zcode.javareceiver.parsing.ZcodeCommandChannel;

public class ZcodeInterruptVectorOut extends ZcodeOutStream {
    private final ZcodeNotificationManager notificationManager;
    private final ZcodeParameters          params;
    private ZcodeOutStream                 out = null;

    public ZcodeInterruptVectorOut(final ZcodeNotificationManager notificationManager, final ZcodeParameters params) {
        this.notificationManager = notificationManager;
        this.params = params;
    }

    @Override
    public ZcodeOutStream markNotification() {
        return out.markNotification();
    }

    @Override
    public ZcodeOutStream markBroadcast() {
        return out.markBroadcast();
    }

    @Override
    public ZcodeOutStream writeStatus(final ZcodeResponseStatus st) {
        return out.writeStatus(st);
    }

    @Override
    public ZcodeOutStream writeField(final char f, final byte v) {
        return out.writeField(f, v);
    }

    @Override
    public ZcodeOutStream continueField(final byte v) {
        return out.continueField(v);
    }

    @Override
    public ZcodeOutStream writeBigHexField(final byte[] value, final int length) {
        return out.writeBigHexField(value, length);
    }

    @Override
    public ZcodeOutStream writeBigStringField(final byte[] value, final int length) {
        return out.writeBigStringField(value, length);
    }

    @Override
    public ZcodeOutStream writeBigStringField(final String s) {
        return out.writeBigStringField(s);
    }

    @Override
    public ZcodeOutStream writeBytes(final byte[] value, final int length) {
        return out.writeBytes(value, length);
    }

    @Override
    public ZcodeOutStream writeCommandSeparator() {
        return out.writeCommandSeparator();
    }

    @Override
    public ZcodeOutStream writeCommandSequenceSeparator() {
        return out.writeCommandSequenceSeparator();
    }

    @Override
    public ZcodeOutStream writeCommandSequenceErrorHandler() {
        return out.writeCommandSequenceErrorHandler();
    }

    @Override
    public void openResponse(final ZcodeCommandChannel target) {
        final ZcodeInterruptVectorChannel channel = (ZcodeInterruptVectorChannel) target;
        if (out.isOpen()) {
            out.close();
        }
        out.mostRecent = this;
        out.openNotification(notificationManager.getNotificationChannel());
        out.markNotification();
        out.writeField(Zchars.NOTIFY_TYPE_PARAM.ch, (byte) 1);
        out.writeField('A', (byte) 1);
        out.writeField('T', channel.getInterrupt().getNotificationType());
        out.writeField('I', channel.getInterrupt().getNotificationBus());
        out.writeStatus(ZcodeResponseStatus.OK);
        if (params.findInterruptSourceAddress && channel.getInterrupt().getSource().hasAddress()) {
            out.writeCommandSeparator();
            out.writeField('A', channel.getInterrupt().getFoundAddress());
            if (channel.getInterrupt().hasFindableAddress()) {
                out.writeStatus(ZcodeResponseStatus.OK);
            } else {
                out.writeStatus(ZcodeResponseStatus.CMD_FAIL);
            }
        }
        out.writeCommandSeparator();
        channel.getInterrupt().clear();
    }

    @Override
    public void openNotification(final ZcodeCommandChannel target) {
        throw new UnsupportedOperationException("Interrupt Vector Out streams are response only");
    }

    @Override
    public void openDebug(final ZcodeCommandChannel target) {
        throw new UnsupportedOperationException("Interrupt Vector Out streams are response only");
    }

    @Override
    public boolean isOpen() {
        return out.isOpen() && out.mostRecent == this;
    }

    @Override
    public void close() {
        out.close();
    }

    @Override
    public boolean lock() {
        if (out == null) {
            out = notificationManager.getNotificationChannel().acquireOutStream();
        }
        return out.lock();
    }

    @Override
    public boolean isLocked() {
        if (out == null) {
            out = notificationManager.getNotificationChannel().acquireOutStream();
        }
        return out.isLocked();
    }

    @Override
    public void unlock() {
        out.unlock();
        notificationManager.getNotificationChannel().releaseOutStream();
        out = null;
    }

}
