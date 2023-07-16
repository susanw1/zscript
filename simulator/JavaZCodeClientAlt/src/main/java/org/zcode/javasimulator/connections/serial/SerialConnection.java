package org.zcode.javasimulator.connections.serial;

import org.zcode.javasimulator.BlankCommunicationResponse;
import org.zcode.javasimulator.CommunicationPacket;
import org.zcode.javasimulator.CommunicationResponse;
import org.zcode.javasimulator.Connection;
import org.zcode.javasimulator.Entity;
import org.zcode.javasimulator.ProtocolConnection;

public final class SerialConnection extends ProtocolConnection<SerialProtocolCategory, SerialConnection> {
	private Entity e1;
	private Entity e2;

	public SerialConnection(Connection<SerialProtocolCategory> connection) {
		super(SerialProtocolCategory.class, connection);
	}

	public void addEndpoint(Entity endpoint) {
		if (e1 == null) {
			e1 = endpoint;
		} else if (e2 == null) {
			e2 = endpoint;
		} else {
			throw new IllegalStateException("Serial Connection cannot have more than two participants");
		}
	}

	@Override
	public CommunicationResponse<SerialConnection> sendMessage(Entity source, CommunicationPacket<SerialConnection> packet) {
		if (packet.getClass() != SerialPacket.class) {
			throw new IllegalArgumentException("Unknown packet type for SPI protocol connection: " + packet.getClass().getName());
		}
		if (source != e1 && source != e2) {
			throw new IllegalArgumentException("Entity must be part of Serial Connection to send messages");
		}
		Entity target;
		if (source == e1) {
			target = e2;
		} else {
			target = e1;
		}
		if (target == null) {
			return new BlankCommunicationResponse<>(SerialConnection.class);
		}
		return target.receive(connection, packet);
	}

	@Override
	public void mergeFrom(SerialConnection other) {
		if (other.e1 != null) {
			addEndpoint(other.e1);
		}
		if (other.e2 != null) {
			addEndpoint(e2);
		}
	}

}
