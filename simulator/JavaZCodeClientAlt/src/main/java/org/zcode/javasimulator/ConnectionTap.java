package org.zcode.javasimulator;

public interface ConnectionTap {
	void messagePassed(Connection<?> con, CommunicationPacket<?> packet, CommunicationResponse<?> response);
}
