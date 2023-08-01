package net.zscript.javasimulator;

public interface ConnectionTap {
	void messagePassed(Connection<?> con, CommunicationPacket<?> packet, CommunicationResponse<?> response);
}
