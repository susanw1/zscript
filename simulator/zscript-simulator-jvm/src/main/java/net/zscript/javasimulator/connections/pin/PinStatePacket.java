package net.zscript.javasimulator.connections.pin;

import net.zscript.javasimulator.BlankCommunicationResponse;
import net.zscript.javasimulator.CommunicationPacket;
import net.zscript.javasimulator.CommunicationResponse;

public class PinStatePacket implements CommunicationPacket<PinConnection>, CommunicationResponse<PinConnection> {
    private final double state;

    public PinStatePacket(double state) {
        this.state = state;
    }

    @Override
    public Class<PinConnection> getProtocolConnectionType() {
        return PinConnection.class;
    }

    public double getState() {
        return state;
    }

    @Override
    public String prettyPrint() {
        return "Pin state changed to" + state;
    }

    @Override
    public CommunicationResponse<PinConnection> generateResponse(ResponseInfo info) {
        return new BlankCommunicationResponse<>(PinConnection.class);
    }

}
