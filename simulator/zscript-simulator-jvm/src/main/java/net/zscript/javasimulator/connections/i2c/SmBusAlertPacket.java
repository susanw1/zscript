package net.zscript.javasimulator.connections.i2c;

import net.zscript.javasimulator.BlankCommunicationResponse;
import net.zscript.javasimulator.CommunicationPacket;
import net.zscript.javasimulator.CommunicationResponse;

public class SmBusAlertPacket implements CommunicationPacket<SmBusAlertConnection>, CommunicationResponse<SmBusAlertConnection> {
    private final boolean alert;

    public SmBusAlertPacket(boolean alert) {
        this.alert = alert;
    }

    public boolean isAlertSet() {
        return alert;
    }

    @Override
    public Class<SmBusAlertConnection> getProtocolConnectionType() {
        return SmBusAlertConnection.class;
    }

    @Override
    public String prettyPrint() {
        return "SmBusAlert: " + alert;
    }

    @Override
    public CommunicationResponse<SmBusAlertConnection> generateResponse(ResponseInfo info) {
        return new BlankCommunicationResponse<>(SmBusAlertConnection.class);
    }

}
