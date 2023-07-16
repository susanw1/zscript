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
import org.zcode.javasimulator.connections.pin.PinStatePacket;
import org.zcode.javasimulator.connections.pin.PinWritePacket;

public class LED extends EntityController {
	private double brightness = 0;

	public LED() {
		Connection<PinProtocolCategory> line = entity.connectNew(PinProtocolCategory.class);
		PinConnection pinCon = new PinConnection(line);
		pinCon.addChild(entity);
		line.addProtocol(pinCon);
		line.sendMessage(entity, PinWritePacket.digitalDrive5V(0, 4700)); // applies a 4k7 ish pulldown.
	}

	@SuppressWarnings("unchecked")
	@Override
	public <U extends ProtocolCategory, T extends ProtocolConnection<U, T>> CommunicationResponse<T> acceptIncoming(Class<U> type, int index,
			CommunicationPacket<T> packet) {
		if (packet.getProtocolConnectionType() == PinConnection.class) {
			brightness = ((PinStatePacket) packet).getState() / 5;
			System.out.println("Brightness set to: " + brightness);
			return (CommunicationResponse<T>) new BlankCommunicationResponse<PinConnection>(PinConnection.class);
		}
		throw new IllegalArgumentException("Invalid communication for LED: " + packet.getProtocolConnectionType().getName());
	}

	public double getBrightness() {
		return brightness;
	}

}
