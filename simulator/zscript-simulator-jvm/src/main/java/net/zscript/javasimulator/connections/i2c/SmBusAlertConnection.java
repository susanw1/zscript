package net.zscript.javasimulator.connections.i2c;

import java.util.HashSet;
import java.util.Set;

import net.zscript.javasimulator.BlankCommunicationResponse;
import net.zscript.javasimulator.CommunicationPacket;
import net.zscript.javasimulator.CommunicationResponse;
import net.zscript.javasimulator.Connection;
import net.zscript.javasimulator.Entity;
import net.zscript.javasimulator.ProtocolConnection;

public final class SmBusAlertConnection extends ProtocolConnection<I2cProtocolCategory, SmBusAlertConnection> {
    public static final I2cAddress ALERT_ADDRESS = new I2cAddress(0x18);
    private Set<Entity> alertSources = new HashSet<>();
    private Entity alertReceiver = null;

    public SmBusAlertConnection(Connection<I2cProtocolCategory> connection) {
        super(I2cProtocolCategory.class, connection);
    }

    public void setAlertReceiver(Entity alertReceiver) {
        this.alertReceiver = alertReceiver;
    }

    @Override
    public CommunicationResponse<SmBusAlertConnection> sendMessage(Entity source, CommunicationPacket<SmBusAlertConnection> packet) {
        if (packet.getClass() == SmBusAlertPacket.class) {
            if (((SmBusAlertPacket) packet).isAlertSet()) {
                alertSources.add(source);
            } else {
                alertSources.remove(source);
            }
            if (alertReceiver != null) {
                return alertReceiver.receive(connection, new SmBusAlertPacket(!alertSources.isEmpty()));
            } else {
                return new BlankCommunicationResponse<SmBusAlertConnection>(SmBusAlertConnection.class);
            }
        } else if (packet.getClass() == SmBusAlertQuery.class) {
            return new SmBusAlertPacket(!alertSources.isEmpty());
        } else {
            throw new IllegalArgumentException("Unknown packet type for SmBus Alert connection: " + packet.getClass().getName());
        }
    }

    @Override
    public void mergeFrom(SmBusAlertConnection other) {
        if (alertReceiver != null && other.alertReceiver != null && alertReceiver != other.alertReceiver) {
            throw new IllegalStateException("Cannot have SmBus Alert systems with multiple alert receivers");
        } else if (alertReceiver == null) {
            this.alertReceiver = other.alertReceiver;
        }
    }

}
