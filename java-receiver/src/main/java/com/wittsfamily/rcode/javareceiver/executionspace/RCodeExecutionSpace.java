package com.wittsfamily.rcode.javareceiver.executionspace;

import com.wittsfamily.rcode.javareceiver.RCodeNotificationManager;
import com.wittsfamily.rcode.javareceiver.RCodeParameters;
import com.wittsfamily.rcode.javareceiver.parsing.RCodeCommandChannel;

public class RCodeExecutionSpace {
    private final RCodeNotificationManager notifications;
    private final byte[] space;
    private int length = 0;
    private int delay = 0;
    private boolean running = false;
    private boolean failed = false;

    private RCodeExecutionSpaceSequenceIn[] inStreams;
    private RCodeExecutionSpaceOut[] outStreams;

    public RCodeExecutionSpace(RCodeParameters params, RCodeNotificationManager notifications) {
        this.notifications = notifications;
        this.space = new byte[params.executionLength];
        inStreams = new RCodeExecutionSpaceSequenceIn[params.executionInNum];
        outStreams = new RCodeExecutionSpaceOut[params.executionOutNum];
        for (int i = 0; i < inStreams.length; i++) {
            inStreams[i] = new RCodeExecutionSpaceSequenceIn(this);
        }
        for (int i = 0; i < outStreams.length; i++) {
            outStreams[i] = new RCodeExecutionSpaceOut(params, this);
        }
    }

    public int getDelay() {
        return delay;
    }

    public void setDelay(int delay) {
        this.delay = delay;
    }

    public byte get(int pos) {
        return space[pos];
    }

    public int getLength() {
        return length;
    }

    public boolean hasInStream() {
        for (int i = 0; i < inStreams.length; i++) {
            if (!inStreams[i].isInUse()) {
                return true;
            }
        }
        return false;
    }

    public RCodeExecutionSpaceSequenceIn acquireInStream(int position) {
        for (RCodeExecutionSpaceSequenceIn in : inStreams) {
            if (!in.isInUse()) {
                in.setup(position);
                return in;
            }
        }
        return null;
    }

    public void releaseInStream(RCodeExecutionSpaceSequenceIn stream) {
        stream.release();
    }

    public boolean hasOutStream() {
        for (int i = 0; i < outStreams.length; i++) {
            if (!outStreams[i].isInUse()) {
                return true;
            }
        }
        return false;
    }

    public RCodeExecutionSpaceOut acquireOutStream() {
        for (RCodeExecutionSpaceOut out : outStreams) {
            if (!out.isInUse()) {
                out.setInUse(true);
                return out;
            }
        }
        return null;
    }

    public void releaseOutStream(RCodeExecutionSpaceOut stream) {
        if (!stream.isDataBufferFull()) {
            stream.setInUse(false);
        }
    }

    public void flush() {
        for (int i = 0; i < outStreams.length; i++) {
            if (outStreams[i].isDataBufferFull() && outStreams[i].flush()) {
                outStreams[i].setInUse(false);
            }
        }
    }

    public RCodeCommandChannel getNotificationChannel() {
        return notifications.getNotificationChannel();
    }

    public boolean isRunning() {
        return running;
    }

    public void setRunning(boolean b) {
        running = b;
    }

    public boolean hasFailed() {
        return failed;
    }

    public void setFailed(boolean failed) {
        this.failed = failed;
    }

    public void write(byte[] data, int address, boolean isEnd) {
        for (int i = 0; i < data.length; i++) {
            space[i + address] = data[i];
        }
        if (isEnd) {
            length = address + data.length;
        }
    }
}
