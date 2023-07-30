package org.zcode.javasimulator.connections.i2c;

import org.zcode.javasimulator.CommunicationPacket;

public class SmBusAlertQuery implements CommunicationPacket<SmBusAlertConnection> {

    @Override
    public Class<SmBusAlertConnection> getProtocolConnectionType() {
        return SmBusAlertConnection.class;
    }

    @Override
    public String prettyPrint() {
        return "Query state of SmBusAlert line";
    }

}
