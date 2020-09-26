package com.wittsfamily.rcode.javareceiver;

import java.nio.charset.StandardCharsets;
import java.util.Arrays;

import com.wittsfamily.rcode.javareceiver.parsing.RCodeCommandChannel;

public class RCodeDebugOutput {
    private final RCodeParameters params;
    private final byte[] debugBuffer;
    private RCodeCommandChannel channel = null;
    private int position;

    public RCodeDebugOutput(RCodeParameters params) {
        this.params = params;
        this.debugBuffer = new byte[params.debugBufferLength];
    }

    public void setDebugChannel(RCodeCommandChannel channel) {
        if (this.channel != null) {
            this.channel.releaseFromDebugChannel();
        }
        this.channel = channel;
        this.channel.setAsDebugChannel();
    }

    public void println(String s) {
        if (channel != null) {
            RCodeOutStream stream = channel.getOutStream();
            if (stream.lock()) {
                if (stream.mostRecent == this) {
                    if (!stream.isOpen()) {
                        stream.openDebug(channel);
                    }
                } else {
                    stream.mostRecent = this;
                    if (stream.isOpen()) {
                        stream.close();
                    }
                    stream.openDebug(channel);
                }
                if (position != 0) {
                    flushBuffer(stream);
                }
                stream.writeBytes(new byte[] { '#' }, 1);
                byte[] strBytes = s.getBytes(StandardCharsets.UTF_8);
                stream.writeBytes(strBytes, strBytes.length);
                stream.writeBytes(new byte[] { '\n' }, 1);
                stream.close();
                stream.unlock();
            } else {
                writeToBuffer(s.getBytes(StandardCharsets.UTF_8));
                writeToBuffer(new byte[] { (byte) '\n' });
            }
        }
    }

    public void attemptFlush() {
        if (position != 0 && channel != null && channel.getOutStream().lock()) {
            RCodeOutStream stream = channel.getOutStream();
            if (stream.mostRecent == this) {
                if (!stream.isOpen()) {
                    stream.openDebug(channel);
                }
            } else {
                stream.mostRecent = this;
                if (stream.isOpen()) {
                    stream.close();
                }
                stream.openDebug(channel);
            }
            flushBuffer(channel.getOutStream());
            stream.close();
            stream.unlock();
        }
    }

    private void flushBuffer(RCodeOutStream stream) {
        int curPos = 0;
        int prevPos = 0;
        while (curPos < position) {
            prevPos = curPos;
            while (curPos < params.debugBufferLength && debugBuffer[curPos] != '\n') {
                curPos++;
            }
            stream.writeBytes(new byte[] { '#' }, 1);
            stream.writeBytes(Arrays.copyOfRange(debugBuffer, prevPos, curPos), curPos - prevPos);
            stream.writeBytes(new byte[] { '\n' }, 1);
            curPos++;
        }
        if (position == params.debugBufferLength + 1) {
            byte[] overRunMessage = "#Debug buffer out of space, some data lost\n".getBytes(StandardCharsets.UTF_8);
            stream.writeBytes(overRunMessage, overRunMessage.length);
        }
        position = 0;
    }

    private void writeToBuffer(byte[] b) {
        if (position + b.length >= params.debugBufferLength) {
            int lenToCopy = params.debugBufferLength - position;
            if (lenToCopy > 0) {
                System.arraycopy(b, 0, debugBuffer, position, lenToCopy);
            }
            position = params.debugBufferLength + 1;
        } else {
            System.arraycopy(b, 0, debugBuffer, position, b.length);
            position += b.length;
        }
    }
}
