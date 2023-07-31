package org.zcode.javasimulator.connections.i2c;

import org.zcode.javasimulator.CommunicationPacket;

public abstract class I2cPacket implements CommunicationPacket<I2cConnection> {
    private final I2cAddress address;
    private final long baud;
    private final boolean hasStopBit;

    public I2cPacket(I2cAddress address, long baud, boolean hasStopBit) {
        this.address = address;
        this.baud = baud;
        this.hasStopBit = hasStopBit;
    }

    public I2cAddress getAddress() {
        return address;
    }

    public long getBaud() {
        return baud;
    }

    public boolean hasStopBit() {
        return hasStopBit;
    }

    @Override
    public Class<I2cConnection> getProtocolConnectionType() {
        return I2cConnection.class;
    }

    @Override
    public String prettyPrint() {
        StringBuilder sb = new StringBuilder();

        if (baud != 0) {
            sb.append("\tBaud rate: ");
            if (baud % 1000000 == 0) {
                sb.append(baud / 1000000);
                sb.append("MHz");
            } else if (baud % 1000 == 0) {
                sb.append(baud / 1000);
                sb.append("kHz");
            } else {
                sb.append(baud);
                sb.append("Hz");
            }
            sb.append("\n");
        }

        sb.append("\tAddress: ");
        sb.append(address.prettyPrint());
        sb.append("\n");

        if (!hasStopBit) {
            sb.append("\tNo stop");
            sb.append("\n");
        }

        return sb.toString();
    }

}
