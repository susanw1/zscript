package org.zcode.javasimulator.zcode.i2c;

import java.util.Arrays;

import org.zcode.javareceiver.core.ZcodeCommandOutStream;
import org.zcode.javareceiver.core.ZcodeLockSet;
import org.zcode.javareceiver.core.ZcodeOutStream;
import org.zcode.javareceiver.core.ZcodeStatus;
import org.zcode.javareceiver.notifications.ZcodeNotificationSource;
import org.zcode.javareceiver.tokenizer.Zchars;
import org.zcode.javasimulator.BlankCommunicationResponse;
import org.zcode.javasimulator.CommunicationPacket;
import org.zcode.javasimulator.CommunicationResponse;
import org.zcode.javasimulator.Entity;
import org.zcode.javasimulator.ProtocolConnection;
import org.zcode.javasimulator.connections.i2c.I2cAddress;
import org.zcode.javasimulator.connections.i2c.I2cProtocolCategory;
import org.zcode.javasimulator.connections.i2c.I2cReceivePacket;
import org.zcode.javasimulator.connections.i2c.I2cReceiveResponse;
import org.zcode.javasimulator.connections.i2c.I2cResponse;
import org.zcode.javasimulator.connections.i2c.SmBusAlertConnection;
import org.zcode.javasimulator.connections.i2c.SmBusAlertPacket;
import org.zcode.javasimulator.zcode.ZcodeSimulatorConsumer;

public class I2cNotificationHandler implements ZcodeSimulatorConsumer<I2cProtocolCategory> {
    private static final int CHUNK_LENGTH = 8;
    private ZcodeNotificationSource source = new ZcodeNotificationSource();
    private int notificationSet = 0;
    private int bitSet = 0;
    private int addressingSet = 0;

    public ZcodeNotificationSource getNotificationSource() {
        return source;
    }

    @Override
    public <T extends ProtocolConnection<I2cProtocolCategory, T>> CommunicationResponse<T> acceptPacket(int index, CommunicationPacket<T> packet) {
        if (packet.getClass() == SmBusAlertPacket.class) {
            if (index >= 16) {
                throw new IllegalArgumentException("Too many I2C connections");
            }
            if ((notificationSet & (1 << index)) != 0) {
                if (((SmBusAlertPacket) packet).isAlertSet()) {
                    bitSet |= 1 << index;
                } else {
                    bitSet &= ~(1 << index);
                }
            }
            generateNotification();
            return new BlankCommunicationResponse<T>(packet.getProtocolConnectionType());
        } else {
            throw new IllegalArgumentException("Unknown packet type: " + packet);
        }
    }

    public void setNotifications(int index, boolean notification) {
        if (notification) {
            notificationSet |= 1 << index;
        } else {
            notificationSet &= ~(1 << index);
            bitSet &= ~(1 << index);
        }
    }

    public void setAddressing(int index, boolean addressing) {
        if (addressing) {
            addressingSet |= 1 << index;
        } else {
            addressingSet &= ~(1 << index);
        }
    }

    public void generateNotification() {
        if (source.hasNotification()) {
            return;
        }
        int i;

        for (i = 0; i < 16; i++) {
            if ((bitSet & (1 << i)) != 0) {
                break;
            }
        }
        if (i == 16) {
            return;
        }
        source.set(ZcodeLockSet.allLocked(), 0x50 | i, (addressingSet & (1 << i)) != 0);
    }

    @Override
    public Class<? extends ProtocolConnection<I2cProtocolCategory, ?>>[] getConnections() {
        return new Class[] { SmBusAlertConnection.class };
    }

    public boolean notification(Entity entity, ZcodeOutStream out, int i, boolean isAddressed) {
        ZcodeCommandOutStream commandOut = out.asCommandOutStream();
        bitSet &= ~(1 << i);
        I2cResponse addrRespTmp = (I2cResponse) entity.getConnection(I2cProtocolCategory.class, i).sendMessage(entity, new I2cReceivePacket(SmBusAlertConnection.ALERT_ADDRESS, 2));
        if (!addrRespTmp.worked()) {
            if (isAddressed) {
                commandOut.writeField('S', ZcodeStatus.ADDRESS_NOT_FOUND);
            } else {
                commandOut.writeField('P', i);
            }
            generateNotification();
            return true;
        }
        I2cReceiveResponse addrResp = (I2cReceiveResponse) addrRespTmp;
        byte[] addrData = addrResp.getData();
        boolean tenBit = (addrData[0] & 0x7C) == 0x78;
        int addr;
        if (tenBit) {
            addr = (addrData[0] & 0x3) << 8 | addrData[1];
        } else {
            addr = addrData[0];
        }
        if (!isAddressed) {
            commandOut.writeField('P', i);
            commandOut.writeField('A', addr);
            if (tenBit) {
                commandOut.writeField('N', 0);
            }
            generateNotification();
            return true;
        }
        commandOut.writeField(Zchars.Z_ADDRESSING, 0x5);
        commandOut.writeField(Zchars.Z_ADDRESSING_CONTINUE, (source.getID() & 0xF));
        commandOut.writeField(Zchars.Z_ADDRESSING_CONTINUE, addr | (tenBit ? 0x8000 : 0));
        byte[] data;
        while (true) {
            I2cResponse respTmp = (I2cResponse) entity.getConnection(I2cProtocolCategory.class, i).sendMessage(entity,
                    new I2cReceivePacket(new I2cAddress(addr), CHUNK_LENGTH));
            if (!respTmp.worked()) {
                out.endSequence();
                commandOut.writeField('!', 0x50);
                commandOut.writeField('S', ZcodeStatus.ADDRESS_NOT_FOUND);
                break;
            }
            data = ((I2cReceiveResponse) respTmp).getData();
            int nonNlLen;
            for (nonNlLen = 0; nonNlLen < data.length && data[nonNlLen] != '\n'; nonNlLen++)
                ;
            if (nonNlLen < data.length) {
                out.writeBytes(Arrays.copyOf(data, nonNlLen));
                break;
            } else {
                out.writeBytes(data);
            }
        }
        generateNotification();
        return true;
    }

}
