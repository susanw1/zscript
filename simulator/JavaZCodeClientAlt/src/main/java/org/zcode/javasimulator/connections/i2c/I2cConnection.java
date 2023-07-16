package org.zcode.javasimulator.connections.i2c;

import java.util.HashMap;
import java.util.Map;

import org.zcode.javasimulator.CommunicationPacket;
import org.zcode.javasimulator.CommunicationResponse;
import org.zcode.javasimulator.Connection;
import org.zcode.javasimulator.Entity;
import org.zcode.javasimulator.ProtocolConnection;

public final class I2cConnection extends ProtocolConnection<I2cProtocolCategory, I2cConnection> {
	private final Map<I2cAddress, Entity> entities = new HashMap<>();

	public I2cConnection(Connection<I2cProtocolCategory> connection) {
		super(I2cProtocolCategory.class, connection);
	}

	public void connect(I2cAddress address, Entity entity) {
		entities.put(address, entity);
	}

	@Override
	public CommunicationResponse<I2cConnection> sendMessage(Entity source, CommunicationPacket<I2cConnection> packet) {
		I2cAddress address;
		if (packet.getClass() == I2cSendPacket.class) {
			address = ((I2cSendPacket) packet).getAddress();
		} else if (packet.getClass() == I2cReceivePacket.class) {
			address = ((I2cReceivePacket) packet).getAddress();
		} else {
			throw new IllegalArgumentException("Unknown packet type for I2C protocol connection: " + packet.getClass().getName());
		}

		Entity target = entities.get(address);
		CommunicationResponse<I2cConnection> resp;
		if (target == null) {
			resp = new I2cAddressNackResponse();
		} else {
			resp = target.receive(connection, packet);
		}
		return resp;
	}

	@Override
	public void mergeFrom(I2cConnection other) {
		entities.putAll(other.entities);
	}
}
