package com.wittsfamily.rcode.executionspace;

import com.wittsfamily.rcode.AbstractRCodeOutStream;
import com.wittsfamily.rcode.RCodeOutStream;
import com.wittsfamily.rcode.RCodeParameters;
import com.wittsfamily.rcode.RCodeResponseStatus;
import com.wittsfamily.rcode.parsing.RCodeCommandChannel;

public class RCodeExecutionSpaceOut extends AbstractRCodeOutStream {
    private final RCodeExecutionSpace space;
    private final byte[] buffer;
    private int length = 0;
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
    public RCodeOutStream writeBytes(byte[] value, int length) {
        if (isOpen) {
            if (length == buffer.length) {
                overLength = true;
            }
            for (int i = 0; i < length && !overLength; i++) {
                buffer[this.length++] = value[i];
                if (this.length == buffer.length) {
                    overLength = true;
                }
            }
        }
        return this;
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
    public void openNotification() {
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
            out.openNotification();
            out.mostRecent = space;
            out.markNotification();
            out.writeField('Z', (byte) 2);
            out.writeBytes(buffer, length);
            out.close();
            out.unlock();
            dataBufferFull = false;
            return true;
        } else {
            dataBufferFull = true;
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
