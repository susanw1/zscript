package org.zcode.javasimulator;

public interface CommunicationPacket<T extends ProtocolConnection<?, T>> {

	Class<T> getProtocolConnectionType();

	String prettyPrint();
}
