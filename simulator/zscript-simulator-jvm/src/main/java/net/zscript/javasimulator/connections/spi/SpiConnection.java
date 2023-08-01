package net.zscript.javasimulator.connections.spi;

import java.util.HashMap;
import java.util.Map;

import net.zscript.javasimulator.CommunicationPacket;
import net.zscript.javasimulator.CommunicationResponse;
import net.zscript.javasimulator.Connection;
import net.zscript.javasimulator.Entity;
import net.zscript.javasimulator.ProtocolConnection;

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
