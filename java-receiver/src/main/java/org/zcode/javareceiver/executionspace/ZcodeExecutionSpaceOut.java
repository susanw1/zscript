package org.zcode.javareceiver.executionspace;

import org.zcode.javareceiver.AbstractZcodeOutStream;
import org.zcode.javareceiver.ZcodeOutStream;
import org.zcode.javareceiver.ZcodeParameters;
import org.zcode.javareceiver.ZcodeResponseStatus;
import org.zcode.javareceiver.parsing.ZcodeCommandChannel;

public class ZcodeExecutionSpaceOut extends AbstractZcodeOutStream {
    private final ZcodeExecutionSpace space;
    private final byte[]              buffer;
    private int                       length         = 0;
    private int                       lastEndPos     = 0;
    private boolean                   overLength     = false;
    private boolean                   inUse          = false;
    private boolean                   dataBufferFull = false;
    private boolean                   isOpen         = false;
    private ZcodeResponseStatus       status         = ZcodeResponseStatus.OK;

    public ZcodeExecutionSpaceOut(final ZcodeParameters params, final ZcodeExecutionSpace space) {
        this.space = space;
        this.buffer = new byte[params.executionOutBufferSize];
    }

    @Override
    public void writeByte(final byte value) {
        if (length == buffer.length) {
            overLength = true;
        }
        if (isOpen && !overLength) {
            buffer[length++] = value;
        }
    }

    @Override
    public ZcodeOutStream writeBytes(final byte[] value, final int l) {
        if (isOpen) {
            if (length == buffer.length) {
                overLength = true;
            }
            for (int i = 0; i < l && !overLength; i++) {
                buffer[length++] = value[i];
                if (length == buffer.length) {
                    overLength = true;
                }
            }
        }
        return this;
    }

    @Override
    public ZcodeOutStream writeCommandSeperator() {
        if (!overLength) {
            lastEndPos = length;
        }
        return super.writeCommandSeperator();
    }

    @Override
    public ZcodeOutStream writeStatus(final ZcodeResponseStatus st) {
        if (st != ZcodeResponseStatus.OK) {
            status = st;
        }
        return super.writeStatus(st);
    }

    @Override
    public void openResponse(final ZcodeCommandChannel target) {
        isOpen = true;
        length = 0;
        overLength = false;
    }

    @Override
    public void openNotification(final ZcodeCommandChannel target) {
        throw new UnsupportedOperationException("Execution Space Out streams are response only");
    }

    @Override
    public void openDebug(final ZcodeCommandChannel target) {
        throw new UnsupportedOperationException("Execution Space Out streams are response only");
    }

    @Override
    public boolean isOpen() {
        return isOpen;
    }

    @Override
    public void close() {
        isOpen = false;
        if (status != ZcodeResponseStatus.OK) {
            flush();
        } else {
            inUse = false;
        }
    }

    public boolean flush() {
        if (status != ZcodeResponseStatus.CMD_FAIL) {
            space.setRunning(false);
        }
        if ((!space.getNotificationChannel().hasOutStream() || !space.getNotificationChannel().acquireOutStream().isLocked())) {
            final ZcodeOutStream out = space.getNotificationChannel().acquireOutStream();
            if (out.isOpen()) {
                out.close();
            }
            out.openNotification(space.getNotificationChannel());
            out.mostRecent = space;
            out.markNotification();
            out.writeField('Z', (byte) 2);
            if (overLength) {
                out.writeBytes(buffer, lastEndPos);
                out.writeCommandSeperator();
                out.writeStatus(ZcodeResponseStatus.RESP_TOO_LONG);
                out.writeCommandSeperator();
                out.writeStatus(status);
                out.writeCommandSequenceSeperator();
            } else {
                out.writeBytes(buffer, length);
            }
            out.close();
            out.unlock();
            dataBufferFull = false;
            status = ZcodeResponseStatus.OK;
            return true;
        } else {
            dataBufferFull = true;
            status = ZcodeResponseStatus.OK;
            return false;
        }
    }

    public boolean isDataBufferFull() {
        return dataBufferFull;
    }

    public void setInUse(final boolean inUse) {
        this.inUse = inUse;
    }

    public boolean isInUse() {
        return inUse;
    }

}
