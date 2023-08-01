package net.zscript.javasimulator;

public class BlankCommunicationResponse<T extends ProtocolConnection<?, T>> implements CommunicationResponse<T> {
	private final Class<T> protocolType;

	public BlankCommunicationResponse(Class<T> protocolType) {
		this.protocolType = protocolType;
	}

	@Override
	public String prettyPrint() {
		return "";
	}

	@Override
	public Class<T> getProtocolConnectionType() {
		return protocolType;
	}

}
