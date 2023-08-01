package net.zscript.javasimulator.connections.i2c;

import net.zscript.javasimulator.CommunicationPacket;
import net.zscript.javasimulator.CommunicationResponse;

public class SmBusAlertQuery implements CommunicationPacket<SmBusAlertConnection> {

    @Override
    public Class<SmBusAlertConnection> getProtocolConnectionType() {
        return SmBusAlertConnection.class;
    }

    @Override
    public String prettyPrint() {
        return "Query state of SmBusAlert line";
    }

    private class SmBusAlertQueryResponseInfo implements ResponseInfo {
        private final boolean isAlert;

        public SmBusAlertQueryResponseInfo(boolean isAlert) {
            this.isAlert = isAlert;
        }

        public SmBusAlertPacket build() {
            return new SmBusAlertPacket(isAlert);
        }
    };

    public ResponseInfo value(boolean isAlert) {
        return new SmBusAlertQueryResponseInfo(isAlert);
    }

    @Override
    public CommunicationResponse<SmBusAlertConnection> generateResponse(ResponseInfo info) {
        if (info.getClass() == SmBusAlertQueryResponseInfo.class) {
            return ((SmBusAlertQueryResponseInfo) info).build();
        } else {
            throw new IllegalArgumentException();
        }
    }
}
