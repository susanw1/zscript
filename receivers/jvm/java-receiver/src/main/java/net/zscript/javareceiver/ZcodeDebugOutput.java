package net.zscript.javareceiver;

import java.nio.charset.StandardCharsets;
import java.util.Arrays;

import net.zscript.javareceiver.parsing.ZcodeCommandChannel;

public class ZcodeDebugOutput {
    private final ZcodeParameters params;
    private final byte[]          debugBuffer;
    private ZcodeCommandChannel   channel = null;
    private int                   position;

    public ZcodeDebugOutput(final ZcodeParameters params) {
        this.params = params;
        this.debugBuffer = new byte[params.debugBufferLength];
    }

    public void setDebugChannel(final ZcodeCommandChannel channel) {
        if (this.channel != null) {
            this.channel.releaseFromDebugChannel();
        }
        this.channel = channel;
        this.channel.setAsDebugChannel();
    }

    public void println(final String s) {
        if (channel != null) {
            if ((!channel.hasOutStream() || !channel.acquireOutStream().isLocked())) {
                final ZcodeOutStream stream = channel.acquireOutStream();
                stream.lock();
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
                stream.writeBytes(new byte[] { (byte) Zchars.DEBUG_PREFIX.ch }, 1);
                final byte[] strBytes = s.getBytes(StandardCharsets.UTF_8);
                stream.writeBytes(strBytes, strBytes.length);
                stream.writeBytes(new byte[] { (byte) Zchars.EOL_SYMBOL.ch }, 1);
                stream.close();
                stream.unlock();
                channel.releaseOutStream();
            } else {
                writeToBuffer(s.getBytes(StandardCharsets.UTF_8));
                writeToBuffer(new byte[] { (byte) Zchars.EOL_SYMBOL.ch });
            }
        }
    }

    public void attemptFlush() {
        if (position != 0 && channel != null && (!channel.hasOutStream() || !channel.acquireOutStream().isLocked())) {
            channel.acquireOutStream().lock();
            final ZcodeOutStream stream = channel.acquireOutStream();
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
            flushBuffer(channel.acquireOutStream());
            stream.close();
            stream.unlock();
            channel.releaseOutStream();
        }
    }

    private void flushBuffer(final ZcodeOutStream stream) {
        int curPos  = 0;
        int prevPos = 0;
        while (curPos < position) {
            prevPos = curPos;
            while (curPos < params.debugBufferLength && debugBuffer[curPos] != Zchars.EOL_SYMBOL.ch) {
                curPos++;
            }
            stream.writeBytes(new byte[] { (byte) Zchars.DEBUG_PREFIX.ch }, 1);
            stream.writeBytes(Arrays.copyOfRange(debugBuffer, prevPos, curPos), curPos - prevPos);
            stream.writeBytes(new byte[] { (byte) Zchars.EOL_SYMBOL.ch }, 1);
            curPos++;
        }
        if (position == params.debugBufferLength + 1) {
            final byte[] overRunMessage = "#Debug buffer out of space, some data lost\n".getBytes(StandardCharsets.UTF_8);
            stream.writeBytes(overRunMessage, overRunMessage.length);
        }
        position = 0;
    }

    private void writeToBuffer(final byte[] b) {
        if (position + b.length >= params.debugBufferLength) {
            final int lenToCopy = params.debugBufferLength - position;
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
