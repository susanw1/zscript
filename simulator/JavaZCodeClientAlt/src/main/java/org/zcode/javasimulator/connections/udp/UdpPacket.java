package org.zcode.javasimulator.connections.udp;

import org.zcode.javasimulator.BlankCommunicationResponse;
import org.zcode.javasimulator.CommunicationPacket;
import org.zcode.javasimulator.CommunicationResponse;
import org.zcode.javasimulator.Utility;

public class UdpPacket implements CommunicationPacket<UdpConnection> {
    private final IPAddress address;
    private final long baud;
    private final byte[] data;

    public UdpPacket(IPAddress address, long baud, byte[] data) {
        this.address = address;
        this.baud = baud;
        this.data = data;
    }

    @Override
    public Class<UdpConnection> getProtocolConnectionType() {
        return UdpConnection.class;
    }

    public IPAddress getAddress() {
        return address;
    }

    public long getBaud() {
        return baud;
    }

    public byte[] getData() {
        return data;
    }

    @Override
    public String prettyPrint() {
        StringBuilder sb = new StringBuilder();
        sb.append("{\n");
        sb.append("\tType: UDP packet\n");
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

        sb.append("\tData: ");
        for (int i = 0; i < data.length; i++) {
            sb.append(Utility.toFixedLengthHex(data[i], 2));
            if (i < data.length - 1) {
                sb.append(" ");
            }
        }
        sb.append("\n");

        sb.append("}\n");

        return sb.toString();
    }

    @Override
    public CommunicationResponse<UdpConnection> generateResponse(ResponseInfo info) {
        return new BlankCommunicationResponse<>(UdpConnection.class);
    }

}