package org.zcode.javasimulator.connections.spi;

import java.util.HashMap;
import java.util.Map;

import org.zcode.javasimulator.CommunicationPacket;
import org.zcode.javasimulator.CommunicationResponse;
import org.zcode.javasimulator.Connection;
import org.zcode.javasimulator.Entity;
import org.zcode.javasimulator.ProtocolConnection;

public final class SpiConnection extends ProtocolConnection<SpiProtocolCategory, SpiConnection> {
	private final Map<SpiAddress, Entity> entities = new HashMap<>();

	public SpiConnection(Connection<SpiProtocolCategory> connection) {
		super(SpiProtocolCategory.class, connection);
	}

	@Override
	public CommunicationResponse<SpiConnection> sendMessage(Entity source, CommunicationPacket<SpiConnection> packetIn) {
		if (packetIn.getClass() != SpiPacket.class) {
			throw new IllegalArgumentException("Unknown packet type for SPI protocol connection: " + packetIn.getClass().getName());
		}
		SpiPacket packet = (SpiPacket) packetIn;
		Entity target = entities.get(packet.getAddress());
		CommunicationResponse<SpiConnection> resp;
		if (target == null) {
			resp = new SpiResponse(new byte[packet.getData().length]);
		} else {
			resp = target.receive(connection, packet);
		}
		return resp;
	}

	@Override
	public void mergeFrom(SpiConnection other) {
		entities.putAll(other.entities);
	}

}
