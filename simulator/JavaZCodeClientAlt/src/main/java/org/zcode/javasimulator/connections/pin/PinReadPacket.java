package org.zcode.javasimulator.connections.pin;

import org.zcode.javasimulator.CommunicationPacket;

public class PinReadPacket implements CommunicationPacket<PinConnection> {

	@Override
	public Class<PinConnection> getProtocolConnectionType() {
		return PinConnection.class;
	}

	@Override
	public String prettyPrint() {
		return "Read pin";
	}
}
