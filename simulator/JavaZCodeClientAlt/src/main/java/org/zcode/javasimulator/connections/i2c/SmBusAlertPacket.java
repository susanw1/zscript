package org.zcode.javasimulator.connections.i2c;

import org.zcode.javasimulator.CommunicationPacket;
import org.zcode.javasimulator.CommunicationResponse;

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

}
