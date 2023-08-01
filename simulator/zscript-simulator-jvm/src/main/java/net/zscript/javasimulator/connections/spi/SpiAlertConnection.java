package net.zscript.javasimulator.connections.spi;

import net.zscript.javasimulator.BlankCommunicationResponse;
import net.zscript.javasimulator.CommunicationPacket;
import net.zscript.javasimulator.CommunicationResponse;
import net.zscript.javasimulator.Connection;
import net.zscript.javasimulator.Entity;
import net.zscript.javasimulator.ProtocolConnection;

public final class SpiAlertConnection extends ProtocolConnection<SpiProtocolCategory, SpiAlertConnection> {
	private Entity alertReceiver = null;

	public SpiAlertConnection(Connection<SpiProtocolCategory> connection) {
		super(SpiProtocolCategory.class, connection);
	}

	public void setAlertReceiver(Entity alertReceiver) {
		this.alertReceiver = alertReceiver;
	}

	@Override
	public CommunicationResponse<SpiAlertConnection> sendMessage(Entity source, CommunicationPacket<SpiAlertConnection> packet) {
		if (packet.getClass() == SpiAlertPacket.class) {
			if (alertReceiver != null) {
				return alertReceiver.receive(connection, packet);
			} else {
				return new BlankCommunicationResponse<SpiAlertConnection>(SpiAlertConnection.class);
			}
		} else {
			throw new IllegalArgumentException("Unknown packet type for SPI Alert connection: " + packet.getClass().getName());
		}
	}

	@Override
	public void mergeFrom(SpiAlertConnection other) {
		if (alertReceiver != null && other.alertReceiver != null && alertReceiver != other.alertReceiver) {
			throw new IllegalStateException("Cannot have SPI Alert systems with multiple alert receivers");
		} else if (alertReceiver == null) {
			this.alertReceiver = other.alertReceiver;
		}
	}

}
