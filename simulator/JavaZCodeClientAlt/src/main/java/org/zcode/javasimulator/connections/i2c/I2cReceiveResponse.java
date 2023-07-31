package org.zcode.javasimulator.connections.i2c;

import org.zcode.javasimulator.Utility;

public class I2cReceiveResponse implements I2cResponse {
    private final byte[] data;

    public I2cReceiveResponse(byte[] data) {
        this.data = data;
    }

    @Override
    public Class<I2cConnection> getProtocolConnectionType() {
        return I2cConnection.class;
    }

    @Override
    public boolean worked() {
        return true;
    }

    @Override
    public boolean addressNack() {
        return false;
    }

    public byte[] getData() {
        return data;
    }

    @Override
    public String prettyPrint() {
        StringBuilder sb = new StringBuilder();
        sb.append("{\n");

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

}
