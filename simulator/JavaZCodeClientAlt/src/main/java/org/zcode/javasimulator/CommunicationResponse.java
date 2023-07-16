package org.zcode.javasimulator;

public interface CommunicationResponse<T extends ProtocolConnection<?, T>> {

	Class<T> getProtocolConnectionType();

	String prettyPrint();
}
