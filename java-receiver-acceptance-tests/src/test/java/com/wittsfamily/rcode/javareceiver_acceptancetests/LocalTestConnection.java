package com.wittsfamily.rcode.javareceiver_acceptancetests;

import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.List;
import java.util.function.Consumer;

import com.wittsfamily.rcode.javareceiver.AbstractRCodeOutStream;
import com.wittsfamily.rcode.javareceiver.RCode;
import com.wittsfamily.rcode.javareceiver.RCodeOutStream;
import com.wittsfamily.rcode.javareceiver.RCodeParameters;
import com.wittsfamily.rcode.javareceiver.instreams.RCodeChannelInStream;
import com.wittsfamily.rcode.javareceiver.parsing.RCodeCommandChannel;
import com.wittsfamily.rcode.javareceiver.parsing.RCodeCommandSequence;
import com.wittsfamily.rcode_acceptance_tests.acceptancetest_asserts.RCodeAcceptanceTestConnection;

public class LocalTestConnection implements RCodeAcceptanceTestConnection, RCodeCommandChannel {
    private final List<Consumer<byte[]>> handlers = new ArrayList<>();
    private final RCodeQueueSequenceInStream queueIn = new RCodeQueueSequenceInStream();
    private final RCodeCommandSequence seq;
    private final boolean isPacketBased;
    private final RCodeOutStream out = new AbstractRCodeOutStream() {
        private boolean isOpen = false;

        private List<Byte> current = new ArrayList<>();

        @Override
        public RCodeOutStream writeBytes(byte[] value, int length) {
            for (byte b : value) {
                writeByte(b);
            }
            return this;
        }

        @Override
        public void openResponse(RCodeCommandChannel target) {
            isOpen = true;
        }

        @Override
        public void openNotification(RCodeCommandChannel target) {
            isOpen = true;
        }

        @Override
        public void openDebug(RCodeCommandChannel target) {
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

    public LocalTestConnection(RCode rcode, RCodeParameters params, boolean isPacketBased) {
        this.isPacketBased = isPacketBased;
        this.seq = new RCodeCommandSequence(rcode, params, this);
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
    public RCodeChannelInStream acquireInStream() {
        return queueIn;
    }

    @Override
    public boolean hasOutStream() {
        return true;
    }

    @Override
    public RCodeOutStream acquireOutStream() {
        return out;
    }

    @Override
    public boolean hasCommandSequence() {
        return queueIn.hasNextCommandSequence();
    }

    @Override
    public RCodeCommandSequence getCommandSequence() {
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
