package net.zscript.javasimulator.zcode.i2c;

import java.util.Arrays;

import net.zscript.javareceiver.core.ZscriptCommandOutStream;
import net.zscript.javareceiver.core.LockSet;
import net.zscript.javareceiver.core.OutStream;
import net.zscript.javareceiver.core.ZscriptStatus;
import net.zscript.javareceiver.notifications.ZscriptNotificationSource;
import net.zscript.javareceiver.tokenizer.Zchars;
import net.zscript.javasimulator.BlankCommunicationResponse;
import net.zscript.javasimulator.CommunicationPacket;
import net.zscript.javasimulator.CommunicationResponse;
import net.zscript.javasimulator.Entity;
import net.zscript.javasimulator.ProtocolConnection;
import net.zscript.javasimulator.connections.i2c.I2cAddress;
import net.zscript.javasimulator.connections.i2c.I2cProtocolCategory;
import net.zscript.javasimulator.connections.i2c.I2cReceivePacket;
import net.zscript.javasimulator.connections.i2c.I2cReceiveResponse;
import net.zscript.javasimulator.connections.i2c.I2cResponse;
import net.zscript.javasimulator.connections.i2c.SmBusAlertConnection;
import net.zscript.javasimulator.connections.i2c.SmBusAlertPacket;
import net.zscript.javasimulator.zcode.SimulatorConsumer;

public class I2cNotificationHandler implements SimulatorConsumer<I2cProtocolCategory> {
    private static final int              CHUNK_LENGTH    = 8;
    private final ZscriptNotificationSource source          = new ZscriptNotificationSource();
    private int                           notificationSet = 0;
    private int                           bitSet          = 0;
    private int                           addressingSet   = 0;

    public ZscriptNotificationSource getNotificationSource() {
        return source;
    }

    @Override
    public <T extends ProtocolConnection<I2cProtocolCategory, T>> CommunicationResponse<T> acceptPacket(int index, CommunicationPacket<T> packet) {
        if (packet.getClass() != SmBusAlertPacket.class) {
            throw new IllegalArgumentException("Unknown packet type: " + packet);
        }
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
        source.set(LockSet.allLocked(), 0x50 | i, (addressingSet & (1 << i)) != 0);
    }

    @Override
    public Class<? extends ProtocolConnection<I2cProtocolCategory, ?>>[] getConnections() {
        return new Class[] { SmBusAlertConnection.class };
    }

    public boolean notification(Entity entity, OutStream out, int i, boolean isAddressed) {
        ZscriptCommandOutStream commandOut = out.asCommandOutStream();
        bitSet &= ~(1 << i);
        I2cResponse addrRespTmp = (I2cResponse) entity.getConnection(I2cProtocolCategory.class, i).sendMessage(entity, new I2cReceivePacket(SmBusAlertConnection.ALERT_ADDRESS, 2));
        if (!addrRespTmp.worked()) {
            if (isAddressed) {
                commandOut.writeField('S', ZscriptStatus.ADDRESS_NOT_FOUND);
            } else {
                commandOut.writeField('P', i);
            }
            generateNotification();
            return true;
        }
        I2cReceiveResponse addrResp = (I2cReceiveResponse) addrRespTmp;
        byte[]             addrData = addrResp.getData();
        boolean            tenBit   = (addrData[0] & 0x7C) == 0x78;
        int                addr;
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
                commandOut.writeField('S', ZscriptStatus.ADDRESS_NOT_FOUND);
                break;
            }
            data = ((I2cReceiveResponse) respTmp).getData();
            int nonNlLen;
            for (nonNlLen = 0; nonNlLen < data.length && data[nonNlLen] != '\n'; nonNlLen++) {
                ;
            }
            if (nonNlLen < data.length) {
                out.writeBytes(Arrays.copyOf(data, nonNlLen));
                break;
            }
            out.writeBytes(data);
        }
        generateNotification();
        return true;
    }

}
