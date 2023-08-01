package org.zcode.javareceiver_acceptancetests;

import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.List;
import java.util.function.Consumer;

import org.zcode.zcode_acceptance_tests.acceptancetest_asserts.ZcodeAcceptanceTestConnection;

import net.zscript.javareceiver.AbstractZcodeOutStream;
import net.zscript.javareceiver.Zcode;
import net.zscript.javareceiver.ZcodeOutStream;
import net.zscript.javareceiver.ZcodeParameters;
import net.zscript.javareceiver.instreams.ZcodeChannelInStream;
import net.zscript.javareceiver.parsing.ZcodeCommandChannel;
import net.zscript.javareceiver.parsing.ZcodeCommandSequence;

public class LocalTestConnection implements ZcodeAcceptanceTestConnection, ZcodeCommandChannel {
    private final List<Consumer<byte[]>> handlers = new ArrayList<>();
    private final ZcodeQueueSequenceInStream queueIn = new ZcodeQueueSequenceInStream();
    private final ZcodeCommandSequence seq;
    private final boolean isPacketBased;
    private final ZcodeOutStream out = new AbstractZcodeOutStream() {
        private boolean isOpen = false;

        private List<Byte> current = new ArrayList<>();

        @Override
        public ZcodeOutStream writeBytes(byte[] value, int length) {
            for (byte b : value) {
                writeByte(b);
            }
            return this;
        }

        @Override
        public void openResponse(ZcodeCommandChannel target) {
            isOpen = true;
        }

        @Override
        public void openNotification(ZcodeCommandChannel target) {
            isOpen = true;
        }

        @Override
        public void openDebug(ZcodeCommandChannel target) {
            isOpen = true;
        }

        @Override
        public boolean isOpen() {
            return isOpen;
        }

        @Override
        public void close() {
            byte[] bytes = new byte[current.size()];
            int i = 0;
            for (Byte b : current) {
                bytes[i++] = b;
            }
            try {
                for (Consumer<byte[]> consumer : handlers) {
                    consumer.accept(bytes);
                }
            } catch (Exception e) {
                System.out.println("Failure with: " + StandardCharsets.UTF_8.decode(ByteBuffer.wrap(bytes)).toString());
                e.printStackTrace();
            }
            current.clear();
            isOpen = false;
        }

        @Override
        public void writeByte(byte value) {
            current.add(value);
        }
    };

    public LocalTestConnection(Zcode zcode, ZcodeParameters params, boolean isPacketBased) {
        this.isPacketBased = isPacketBased;
        this.seq = new ZcodeCommandSequence(zcode, params, this);
    }

    @Override
    public void send(byte[] message) {
        queueIn.addMessage(message);
    }

    @Override
    public void registerListener(Consumer<byte[]> handler) {
        handlers.add(handler);
    }

    @Override
    public void clearListeners() {
        handlers.clear();
    }

    @Override
    public boolean hasInStream() {
        return true;
    }

    @Override
    public ZcodeChannelInStream acquireInStream() {
        return queueIn;
    }

    @Override
    public boolean hasOutStream() {
        return true;
    }

    @Override
    public ZcodeOutStream acquireOutStream() {
        return out;
    }

    @Override
    public boolean hasCommandSequence() {
        return queueIn.hasNextCommandSequence();
    }

    @Override
    public ZcodeCommandSequence getCommandSequence() {
        return seq;
    }

    @Override
    public boolean isPacketBased() {
        return isPacketBased;
    }

    @Override
    public void releaseInStream() {
    }

    @Override
    public void releaseOutStream() {
    }

    @Override
    public void setAsNotificationChannel() {
    }

    @Override
    public void setAsDebugChannel() {
    }

    @Override
    public void releaseFromNotificationChannel() {
    }

    @Override
    public void releaseFromDebugChannel() {
    }

    @Override
    public void lock() {
    }

    @Override
    public boolean canLock() {
        return true;
    }

    @Override
    public void unlock() {
    }

}
