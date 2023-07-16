package org.zcode.javasimulator.connections.pin;

import org.zcode.javasimulator.CommunicationPacket;
import org.zcode.javasimulator.CommunicationResponse;

public class PinStatePacket implements CommunicationPacket<PinConnection>, CommunicationResponse<PinConnection> {
	private final double state;

	public PinStatePacket(double state) {
		this.state = state;
	}

	@Override
	public Class<PinConnection> getProtocolConnectionType() {
		return PinConnection.class;
	}

	public double getState() {
		return state;
	}

	@Override
	public String prettyPrint() {
		return "Pin state changed to" + state;
	}
}
