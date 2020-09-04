package com.wittsfamily.rcode.javareceiver.executionspace;

import com.wittsfamily.rcode.javareceiver.AbstractRCodeOutStream;
import com.wittsfamily.rcode.javareceiver.RCodeOutStream;
import com.wittsfamily.rcode.javareceiver.RCodeParameters;
import com.wittsfamily.rcode.javareceiver.RCodeResponseStatus;
import com.wittsfamily.rcode.javareceiver.parsing.RCodeCommandChannel;

public class RCodeExecutionSpaceOut extends AbstractRCodeOutStream {
    private final RCodeExecutionSpace space;
    private final byte[] buffer;
    private int length = 0;
    private int lastEndPos = 0;
    private boolean overLength = false;
    private boolean inUse = false;
    private boolean dataBufferFull = false;
    private boolean isOpen = false;
    private RCodeResponseStatus status = RCodeResponseStatus.OK;

    public RCodeExecutionSpaceOut(RCodeParameters params, RCodeExecutionSpace space) {
        this.space = space;
        this.buffer = new byte[params.executionOutBufferSize];
    }

    @Override
    public void writeByte(byte value) {
        if (length == buffer.length) {
            overLength = true;
        }
        if (isOpen && !overLength) {
            buffer[length++] = value;
        }
    }

    @Override
    public RCodeOutStream writeBytes(byte[] value, int l) {
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
    public RCodeOutStream writeCommandSeperator() {
        if (!overLength) {
            lastEndPos = length;
        }
        return super.writeCommandSeperator();
    }

    @Override
    public RCodeOutStream writeStatus(RCodeResponseStatus st) {
        if (st != RCodeResponseStatus.OK) {
            status = st;
        }
        return super.writeStatus(st);
    }

    @Override
    public void openResponse(RCodeCommandChannel target) {
        isOpen = true;
        length = 0;
        overLength = false;
    }

    @Override
    public void openNotification(RCodeCommandChannel target) {
        throw new UnsupportedOperationException("Execution Space Out streams are response only");
    }

    @Override
    public void openDebug(RCodeCommandChannel target) {
        throw new UnsupportedOperationException("Execution Space Out streams are response only");
    }

    @Override
    public boolean isOpen() {
        return isOpen;
    }

    @Override
    public void close() {
        isOpen = false;
        if (status != RCodeResponseStatus.OK) {
            flush();
        } else {
            inUse = false;
        }
    }

    public boolean flush() {
        if (status != RCodeResponseStatus.CMD_FAIL) {
            space.setRunning(false);
        }
        if (!space.getNotificationChannel().getOutStream().isLocked()) {
            RCodeOutStream out = space.getNotificationChannel().getOutStream();
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
                out.writeStatus(RCodeResponseStatus.RESP_TOO_LONG);
                out.writeCommandSeperator();
                out.writeStatus(status);
                out.writeCommandSequenceSeperator();
            } else {
                out.writeBytes(buffer, length);
            }
            out.close();
            out.unlock();
            dataBufferFull = false;
            status = RCodeResponseStatus.OK;
            return true;
        } else {
            dataBufferFull = true;
            status = RCodeResponseStatus.OK;
            return false;
        }
    }

    public boolean isDataBufferFull() {
        return dataBufferFull;
    }

    public void setInUse(boolean inUse) {
        this.inUse = inUse;
    }

    public boolean isInUse() {
        return inUse;
    }

}
