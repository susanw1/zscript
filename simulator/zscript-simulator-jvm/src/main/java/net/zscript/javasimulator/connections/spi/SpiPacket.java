package net.zscript.javasimulator.connections.spi;

import net.zscript.javasimulator.CommunicationPacket;
import net.zscript.javasimulator.CommunicationResponse;
import net.zscript.javasimulator.Utility;

public class SpiPacket implements CommunicationPacket<SpiConnection> {
    private final SpiAddress address;
    private final long baud;
    private final byte[] data;

    public SpiPacket(SpiAddress address, long baud, byte[] data) {
        this.address = address;
        this.baud = baud;
        this.data = data;
    }

    @Override
    public Class<SpiConnection> getProtocolConnectionType() {
        return SpiConnection.class;
    }

    public SpiAddress getAddress() {
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
        sb.append("\tType: SPI packet\n");
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
            sb.append(Utility.toFixedLengthHex(Byte.toUnsignedInt(data[i]), 2));
            if (i < data.length - 1) {
                sb.append(" ");
            }
        }
        sb.append("\n");

        sb.append("}\n");

        return sb.toString();
    }

    private class SpiResponseInfo implements ResponseInfo {
        private final byte[] data;

        public SpiResponseInfo(byte[] data) {
            this.data = data;
        }

        public SpiResponse build() {
            return new SpiResponse(data);
        }
    };

    public SpiResponseInfo complete(byte[] data) {
        return new SpiResponseInfo(data);
    }

    @Override
    public CommunicationResponse<SpiConnection> generateResponse(ResponseInfo info) {
        if (info.getClass() == SpiResponseInfo.class) {
            return ((SpiResponseInfo) info).build();
        } else {
            throw new IllegalArgumentException();
        }
    }

}
