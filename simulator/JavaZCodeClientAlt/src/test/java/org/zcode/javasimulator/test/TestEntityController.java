package org.zcode.javasimulator.test;

import org.zcode.javasimulator.BlankCommunicationResponse;
import org.zcode.javasimulator.CommunicationPacket;
import org.zcode.javasimulator.CommunicationResponse;
import org.zcode.javasimulator.Connection;
import org.zcode.javasimulator.EntityController;
import org.zcode.javasimulator.ProtocolCategory;
import org.zcode.javasimulator.ProtocolConnection;
import org.zcode.javasimulator.connections.i2c.I2cAddress;
import org.zcode.javasimulator.connections.i2c.I2cConnection;
import org.zcode.javasimulator.connections.i2c.I2cProtocolCategory;
import org.zcode.javasimulator.connections.i2c.I2cSendPacket;
import org.zcode.javasimulator.connections.i2c.I2cSendResponse;

public class TestEntityController extends EntityController {

	public TestEntityController() {
		Connection<I2cProtocolCategory> connection = entity.connectNew(I2cProtocolCategory.class);
		connection.addProtocol(new I2cConnection(connection));
	}

	public void sendTest() {
		Connection<I2cProtocolCategory> connection = entity.getConnection(I2cProtocolCategory.class, 0);
		connection.sendMessage(entity, new I2cSendPacket(new I2cAddress(0x23), new byte[] { 0x22, 0x44 }));
	}

	@SuppressWarnings("unchecked")
	@Override
	public <U extends ProtocolCategory, T extends ProtocolConnection<U, T>> CommunicationResponse<T> acceptIncoming(Class<U> type, int index,
			CommunicationPacket<T> packet) {
		if (packet.getProtocolConnectionType() == I2cConnection.class) {
			return (CommunicationResponse<T>) I2cSendResponse.DataNack(1);
		}
		return new BlankCommunicationResponse<T>(packet.getProtocolConnectionType());
	}

}
