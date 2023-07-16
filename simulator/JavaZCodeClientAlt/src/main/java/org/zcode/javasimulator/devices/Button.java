package org.zcode.javasimulator.devices;

import org.zcode.javasimulator.BlankCommunicationResponse;
import org.zcode.javasimulator.CommunicationPacket;
import org.zcode.javasimulator.CommunicationResponse;
import org.zcode.javasimulator.Connection;
import org.zcode.javasimulator.EntityController;
import org.zcode.javasimulator.ProtocolCategory;
import org.zcode.javasimulator.ProtocolConnection;
import org.zcode.javasimulator.connections.pin.PinConnection;
import org.zcode.javasimulator.connections.pin.PinProtocolCategory;
import org.zcode.javasimulator.connections.pin.PinWritePacket;

public class Button extends EntityController {

	public Button() {
		Connection<PinProtocolCategory> line = entity.connectNew(PinProtocolCategory.class);
		PinConnection pinCon = new PinConnection(line);
		pinCon.addChild(entity);
		line.addProtocol(pinCon);
		release();
	}

	@SuppressWarnings("unchecked")
	@Override
	public <U extends ProtocolCategory, T extends ProtocolConnection<U, T>> CommunicationResponse<T> acceptIncoming(Class<U> type, int index,
			CommunicationPacket<T> packet) {
		if (packet.getProtocolConnectionType() == PinConnection.class) {
			return (CommunicationResponse<T>) new BlankCommunicationResponse<PinConnection>(PinConnection.class);
		}
		throw new IllegalArgumentException("Invalid communication for LED: " + packet.getProtocolConnectionType().getName());
	}

	public void press() {
		entity.getConnection(PinProtocolCategory.class, 0).sendMessage(entity, PinWritePacket.digitalDrive5V(true));
	}

	public void release() {
		entity.getConnection(PinProtocolCategory.class, 0).sendMessage(entity, PinWritePacket.digitalDrive5V(false));
	}
}
