package org.zcode.javasimulator.connections.i2c;

import org.zcode.javasimulator.CommunicationResponse;

public class I2cReceivePacket extends I2cPacket {
    private final int length;

    public I2cReceivePacket(I2cAddress address, int length) {
        this(address, 0, true, length);
    }

    public I2cReceivePacket(I2cAddress address, long baud, boolean hasStopBit, int length) {
        super(address, baud, hasStopBit);
        this.length = length;
        if (length <= 0) {
            throw new IllegalArgumentException("I2C receives must have length >=1");
        }
    }

    public int getLength() {
        return length;
    }

    @Override
    public String prettyPrint() {
        StringBuilder sb = new StringBuilder();
        sb.append("{\n");
        sb.append("\tType: I2C receive\n");

        sb.append(super.prettyPrint());

        sb.append("\tLength: ");
        sb.append(length);
        sb.append("\n");

        sb.append("}\n");

        return sb.toString();
    }

    private class I2cReceiveResponseInfo implements ResponseInfo {
        private final boolean isAddrNack;
        private final byte[] data;

        public I2cReceiveResponseInfo(boolean isAddrNack, byte[] data) {
            this.isAddrNack = isAddrNack;
            this.data = data;
        }

        public I2cResponse build() {
            if (isAddrNack) {
                return new I2cAddressNackResponse();
            } else {
                return new I2cReceiveResponse(data);
            }
        }
    };

    public ResponseInfo success(byte[] data) {
        return new I2cReceiveResponseInfo(false, data);
    }

    public ResponseInfo addressNack() {
        return new I2cReceiveResponseInfo(true, null);
    }

    @Override
    public CommunicationResponse<I2cConnection> generateResponse(ResponseInfo info) {
        if (info.getClass() == I2cReceiveResponseInfo.class) {
            return ((I2cReceiveResponseInfo) info).build();
        } else {
            throw new IllegalArgumentException();
        }
    }

}
