package org.zcode.javasimulator.connections.pin;

import org.zcode.javasimulator.BlankCommunicationResponse;
import org.zcode.javasimulator.CommunicationPacket;
import org.zcode.javasimulator.CommunicationResponse;

public class PinReadPacket implements CommunicationPacket<PinConnection> {

    @Override
    public Class<PinConnection> getProtocolConnectionType() {
        return PinConnection.class;
    }

    @Override
    public String prettyPrint() {
        return "Read pin";
    }

    @Override
    public CommunicationResponse<PinConnection> generateResponse(ResponseInfo info) {
        return new BlankCommunicationResponse<>(PinConnection.class);
    }
}
