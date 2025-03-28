package net.zscript.javasimulator.zcode.i2c;

import javax.annotation.Nonnull;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.UncheckedIOException;
import java.util.Arrays;
import java.util.Queue;
import java.util.concurrent.ConcurrentLinkedQueue;

import net.zscript.javareceiver.core.AbstractOutStream;
import net.zscript.javareceiver.core.SequenceOutStream;
import net.zscript.javareceiver.core.ZscriptChannel;
import net.zscript.javareceiver.execution.CommandContext;
import net.zscript.javasimulator.CommunicationPacket;
import net.zscript.javasimulator.CommunicationResponse;
import net.zscript.javasimulator.Entity;
import net.zscript.javasimulator.ProtocolConnection;
import net.zscript.javasimulator.connections.i2c.I2cAddress;
import net.zscript.javasimulator.connections.i2c.I2cConnection;
import net.zscript.javasimulator.connections.i2c.I2cProtocolCategory;
import net.zscript.javasimulator.connections.i2c.I2cReceivePacket;
import net.zscript.javasimulator.connections.i2c.I2cReceiveResponse;
import net.zscript.javasimulator.connections.i2c.I2cSendPacket;
import net.zscript.javasimulator.connections.i2c.I2cSendResponse;
import net.zscript.javasimulator.connections.i2c.SmBusAlertConnection;
import net.zscript.javasimulator.connections.i2c.SmBusAlertPacket;
import net.zscript.javasimulator.zcode.SimulatorConsumer;
import net.zscript.tokenizer.TokenBuffer;
import net.zscript.tokenizer.TokenRingBuffer;
import net.zscript.tokenizer.Tokenizer;

public class I2cChannel extends ZscriptChannel implements SimulatorConsumer<I2cProtocolCategory> {
    private final Entity        e;
    private final I2cAddress    addr;
    private final Tokenizer     in;
    private final Queue<byte[]> outQueue;
    private       int           cachePos = 0;

    public static I2cChannel build(Entity e, I2cAddress addr, int index, int size) {
        Queue<byte[]> outQueue = new ConcurrentLinkedQueue<>();
        I2cChannel channel = new I2cChannel(e, addr, TokenRingBuffer.createBufferWithCapacity(size), outQueue, new AbstractOutStream() {
            private ByteArrayOutputStream stream;

            @Override
            public void open() {
                stream = new ByteArrayOutputStream();
            }

            @Override
            public void close() {
                outQueue.add(stream.toByteArray());
                e.getConnection(I2cProtocolCategory.class, index).getProtocol(I2cConnection.class).connect(SmBusAlertConnection.ALERT_ADDRESS, e);
                e.getConnection(I2cProtocolCategory.class, index).sendMessage(e, new SmBusAlertPacket(true));
                stream = null;
            }

            @Override
            public boolean isOpen() {
                return stream != null;
            }

            @Override
            protected void writeBytes(@Nonnull byte[] bytes, int count, boolean hexMode) {
                if (hexMode) {
                    for (byte b : bytes) {
                        stream.write(toHexChar(b >>> 4));
                        stream.write(toHexChar(b & 0xf));
                    }
                } else {
                    try {
                        stream.write(Arrays.copyOf(bytes, count));
                    } catch (IOException e) {
                        throw new UncheckedIOException(e);
                    }
                }
            }
        });
        e.getConnection(I2cProtocolCategory.class, index).getProtocol(I2cConnection.class).connect(addr, e);
        return channel;
    }

    private I2cChannel(Entity e, I2cAddress addr, TokenBuffer buffer, Queue<byte[]> outQueue, SequenceOutStream out) {
        super(buffer, out);
        this.e = e;
        this.addr = addr;
        this.in = new Tokenizer(buffer.getTokenWriter(), false);
        this.outQueue = outQueue;
    }

    @Override
    public <T extends ProtocolConnection<I2cProtocolCategory, T>> CommunicationResponse<T> acceptPacket(int index, CommunicationPacket<T> packet) {
        if (packet.getClass() == I2cSendPacket.class) {
            byte[] data = ((I2cSendPacket) packet).getData();
            for (byte b : data) {
                //                System.out.print(b + "  " + (char) b + "  ");
                in.accept(b);
            }
            return (CommunicationResponse<T>) I2cSendResponse.Success();
        }
        if (packet.getClass() != I2cReceivePacket.class) {
            throw new IllegalArgumentException("Unknown packet type: " + packet);
        }
        if (((I2cReceivePacket) packet).getAddress().equals(SmBusAlertConnection.ALERT_ADDRESS)) {
            byte[] data = new byte[((I2cReceivePacket) packet).getLength()];
            if (addr.isTenBit()) {
                int i = addr.getAddress();
                if (data.length > 1) {
                    data[1] = (byte) i;
                }
                data[0] = (byte) (0x78 | (i >> 8));
            } else {
                data[0] = (byte) addr.getAddress();
            }
            return (CommunicationResponse<T>) new I2cReceiveResponse(data);
        }
        int    length    = ((I2cReceivePacket) packet).getLength();
        byte[] data      = new byte[length];
        int    dataIndex = 0;
        byte[] cache     = null;
        if (!outQueue.isEmpty()) {
            cache = outQueue.peek();
        }
        while (!outQueue.isEmpty() && dataIndex < length) {
            if (cachePos == cache.length) {
                outQueue.poll();
                cache = outQueue.peek();
                cachePos = 0;
                break;
            }
            data[dataIndex++] = cache[cachePos++];
        }
        if (cache != null && cachePos == cache.length) {
            outQueue.poll();
            cache = outQueue.peek();
            cachePos = 0;
        }
        if (outQueue.isEmpty()) {
            e.getConnection(I2cProtocolCategory.class, index).sendMessage(e, new SmBusAlertPacket(false));
            e.getConnection(I2cProtocolCategory.class, index).getProtocol(I2cConnection.class).disconnect(SmBusAlertConnection.ALERT_ADDRESS, e);
        }
        return (CommunicationResponse<T>) new I2cReceiveResponse(data);
    }

    @Override
    public Class<? extends ProtocolConnection<I2cProtocolCategory, ?>>[] getConnections() {
        return new Class[] { I2cConnection.class };
    }

    @Override
    public void channelInfo(@Nonnull CommandContext ctx) {
        // TODO Auto-generated method stub

    }

    @Override
    public void channelSetup(@Nonnull CommandContext ctx) {
        // TODO Auto-generated method stub

    }

}
