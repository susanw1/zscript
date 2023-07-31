package org.zcode.javasimulator.test;

import org.zcode.javasimulator.CommunicationPacket;
import org.zcode.javasimulator.CommunicationResponse;
import org.zcode.javasimulator.Connection;
import org.zcode.javasimulator.ConnectionTap;

public class PrintingConnectionTap implements ConnectionTap {

	@Override
	public void messagePassed(Connection<?> con, CommunicationPacket<?> packet, CommunicationResponse<?> response) {
		System.out.println("Packet: " + packet.prettyPrint());
		String resp = response.prettyPrint();
		if (resp.length() > 0) {
			System.out.println("Response: " + response.prettyPrint());
		}

	}

}
