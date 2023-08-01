package net.zscript.javasimulator.test;

import net.zscript.javasimulator.CommunicationPacket;
import net.zscript.javasimulator.CommunicationResponse;
import net.zscript.javasimulator.Connection;
import net.zscript.javasimulator.ConnectionTap;

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
