package org.zcode.javasimulator.connections.spi;

import org.zcode.javasimulator.CommunicationPacket;

public class SpiAlertPacket implements CommunicationPacket<SpiAlertConnection> {

	@Override
	public Class<SpiAlertConnection> getProtocolConnectionType() {
		return SpiAlertConnection.class;
	}

	@Override
	public String prettyPrint() {
		return "SpiAlert";
	}

}
