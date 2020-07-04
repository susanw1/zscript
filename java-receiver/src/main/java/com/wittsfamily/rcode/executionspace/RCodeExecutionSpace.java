package com.wittsfamily.rcode.executionspace;

import com.wittsfamily.rcode.RCodeNotificationManager;
import com.wittsfamily.rcode.RCodeParameters;
import com.wittsfamily.rcode.parsing.RCodeCommandChannel;

public class RCodeExecutionSpace {
    private final RCodeNotificationManager notifications;
    private final byte[] space;
    private int length = 0;
    private boolean isRunning = false;

    private RCodeExecutionSpaceSequenceIn[] inStreams;
    private RCodeExecutionSpaceOut[] outStreams;

    public RCodeExecutionSpace(RCodeParameters params, RCodeNotificationManager notifications, byte[] space) {
        this.notifications = notifications;
        this.space = space;
        inStreams = new RCodeExecutionSpaceSequenceIn[params.executionInNum];
        outStreams = new RCodeExecutionSpaceOut[params.executionOutNum];
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
}
