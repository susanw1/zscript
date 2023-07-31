package org.zcode.javasimulator.connections.spi;

import org.zcode.javasimulator.BlankCommunicationResponse;
import org.zcode.javasimulator.CommunicationPacket;
import org.zcode.javasimulator.CommunicationResponse;

public class SpiAlertPacket implements CommunicationPacket<SpiAlertConnection> {

    @Override
    public Class<SpiAlertConnection> getProtocolConnectionType() {
        return SpiAlertConnection.class;
    }

    @Override
    public String prettyPrint() {
        return "SpiAlert";
    }

    @Override
    public CommunicationResponse<SpiAlertConnection> generateResponse(ResponseInfo info) {
        return new BlankCommunicationResponse<>(SpiAlertConnection.class);
    }

}
