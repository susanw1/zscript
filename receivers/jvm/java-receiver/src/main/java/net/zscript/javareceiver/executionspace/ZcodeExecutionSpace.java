package net.zscript.javareceiver.executionspace;

import net.zscript.javareceiver.ZcodeNotificationManager;
import net.zscript.javareceiver.ZcodeParameters;
import net.zscript.javareceiver.parsing.ZcodeCommandChannel;

public class ZcodeExecutionSpace {
    private final ZcodeNotificationManager notifications;
    private final byte[]                   space;
    private int                            length  = 0;
    private int                            delay   = 0;
    private boolean                        running = false;
    private boolean                        failed  = false;

    private final ZcodeExecutionSpaceInStream[] inStreams;
    private final ZcodeExecutionSpaceOut[]      outStreams;

    public ZcodeExecutionSpace(final ZcodeParameters params, final ZcodeNotificationManager notifications) {
        this.notifications = notifications;
        this.space = new byte[params.executionLength];
        inStreams = new ZcodeExecutionSpaceInStream[params.executionInNum];
        outStreams = new ZcodeExecutionSpaceOut[params.executionOutNum];
        for (int i = 0; i < inStreams.length; i++) {
            inStreams[i] = new ZcodeExecutionSpaceInStream(this);
        }
        for (int i = 0; i < outStreams.length; i++) {
            outStreams[i] = new ZcodeExecutionSpaceOut(params, this);
        }
    }

    public int getDelay() {
        return delay;
    }

    public void setDelay(final int delay) {
        this.delay = delay;
    }

    public byte get(final int pos) {
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

    public ZcodeExecutionSpaceInStream acquireInStream(final int position) {
        for (final ZcodeExecutionSpaceInStream in : inStreams) {
            if (!in.isInUse()) {
                in.setup(position);
                return in;
            }
        }
        return null;
    }

    public void releaseInStream(final ZcodeExecutionSpaceInStream stream) {
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

    public ZcodeExecutionSpaceOut acquireOutStream() {
        for (final ZcodeExecutionSpaceOut out : outStreams) {
            if (!out.isInUse()) {
                out.setInUse(true);
                return out;
            }
        }
        return null;
    }

    public void releaseOutStream(final ZcodeExecutionSpaceOut stream) {
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

    public ZcodeCommandChannel getNotificationChannel() {
        return notifications.getNotificationChannel();
    }

    public boolean isRunning() {
        return running;
    }

    public void setRunning(final boolean b) {
        running = b;
    }

    public boolean hasFailed() {
        return failed;
    }

    public void setFailed(final boolean failed) {
        this.failed = failed;
    }

    public void write(final byte[] data, final int address, final boolean isEnd) {
        for (int i = 0; i < data.length; i++) {
            space[i + address] = data[i];
        }
        if (isEnd) {
            length = address + data.length;
        }
    }
}
