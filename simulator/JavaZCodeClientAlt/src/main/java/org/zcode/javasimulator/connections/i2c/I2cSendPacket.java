package org.zcode.javasimulator.connections.i2c;

import org.zcode.javasimulator.Utility;

public class I2cSendPacket extends I2cPacket {

    private final byte[] data;

    public I2cSendPacket(I2cAddress address, byte[] data) {
        this(address, 0, true, data);
    }

    public I2cSendPacket(I2cAddress address, long baud, boolean hasStopBit, byte[] data) {
        super(address, baud, hasStopBit);
        this.data = data;
    }

    public byte[] getData() {
        return data;
    }

    @Override
    public String prettyPrint() {
        StringBuilder sb = new StringBuilder();
        sb.append("{\n");
        sb.append("\tType: I2C send\n");
        sb.append(super.prettyPrint());

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

    private class I2cSendResponseInfo implements ResponseInfo {
        private final boolean isAddrNack;
        private final int dataNackIndex;

        public I2cSendResponseInfo(boolean isAddrNack, int dataNackIndex) {
            this.isAddrNack = isAddrNack;
            this.dataNackIndex = dataNackIndex;
        }

        public I2cResponse build() {
            if (isAddrNack) {
                return new I2cAddressNackResponse();
            } else if (dataNackIndex != -1) {
                return I2cSendResponse.DataNack(dataNackIndex);
            } else {
                return I2cSendResponse.Success();
            }
        }
    };

    public ResponseInfo success() {
        return new I2cSendResponseInfo(false, -1);
    }

    public ResponseInfo addressNack() {
        return new I2cSendResponseInfo(true, 0);
    }

    public ResponseInfo dataNack(int dataNackIndex) {
        return new I2cSendResponseInfo(false, dataNackIndex);
    }

    @Override
    public I2cResponse generateResponse(ResponseInfo info) {
        if (info.getClass() == I2cSendResponseInfo.class) {
            return ((I2cSendResponseInfo) info).build();
        } else {
            throw new IllegalArgumentException();
        }
    }

}
