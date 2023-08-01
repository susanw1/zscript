package net.zscript.javasimulator.connections.udp;

import java.util.HashMap;
import java.util.Map;

import net.zscript.javasimulator.BlankCommunicationResponse;
import net.zscript.javasimulator.CommunicationPacket;
import net.zscript.javasimulator.CommunicationResponse;
import net.zscript.javasimulator.Connection;
import net.zscript.javasimulator.Entity;
import net.zscript.javasimulator.ProtocolConnection;

public final class UdpConnection extends ProtocolConnection<UdpProtocolCategory, UdpConnection> {
	private final Map<IPAddress, Entity> entities = new HashMap<>();

	public UdpConnection(Connection<UdpProtocolCategory> connection) {
		super(UdpProtocolCategory.class, connection);
	}

	public void connect(IPAddress address, Entity entity) {
		entities.put(address, entity);
	}

	@Override
	public CommunicationResponse<UdpConnection> sendMessage(Entity source, CommunicationPacket<UdpConnection> packetIn) {
		if (packetIn.getClass() != UdpPacket.class) {
			throw new IllegalArgumentException("Unknown packet type for UDP protocol connection: " + packetIn.getClass().getName());
		}
		UdpPacket packet = (UdpPacket) packetIn;
		Entity target = entities.get(packet.getAddress());

		if (target == null) {
			return new BlankCommunicationResponse<UdpConnection>(UdpConnection.class);
		} else {
			return target.receive(connection, packet);
		}
	}

	@Override
	public void mergeFrom(UdpConnection other) {
		entities.putAll(other.entities);
	}
}
