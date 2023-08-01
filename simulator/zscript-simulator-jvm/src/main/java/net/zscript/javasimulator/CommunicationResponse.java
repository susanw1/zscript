package net.zscript.javasimulator;

public interface CommunicationResponse<T extends ProtocolConnection<?, T>> {

	Class<T> getProtocolConnectionType();

	String prettyPrint();
}
