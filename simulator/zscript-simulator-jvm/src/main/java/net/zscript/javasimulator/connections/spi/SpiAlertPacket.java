package net.zscript.javasimulator.connections.spi;

import net.zscript.javasimulator.BlankCommunicationResponse;
import net.zscript.javasimulator.CommunicationPacket;
import net.zscript.javasimulator.CommunicationResponse;

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
