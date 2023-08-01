package net.zscript.javasimulator.connections.pin;

import net.zscript.javasimulator.BlankCommunicationResponse;
import net.zscript.javasimulator.CommunicationPacket;
import net.zscript.javasimulator.CommunicationResponse;

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
