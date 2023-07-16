package org.zcode.javasimulator.connections.spi;

import org.zcode.javasimulator.CommunicationResponse;
import org.zcode.javasimulator.Utility;

public class SpiResponse implements CommunicationResponse<SpiConnection> {
	private final byte[] data;

	public SpiResponse(byte[] data) {
		this.data = data;
	}

	@Override
	public Class<SpiConnection> getProtocolConnectionType() {
		return SpiConnection.class;
	}

	public byte[] getData() {
		return data;
	}

	@Override
	public String prettyPrint() {
		StringBuilder sb = new StringBuilder();
		sb.append("{\n");

		sb.append("\tData: ");
		for (int i = 0; i < data.length; i++) {
			sb.append(Utility.toFixedLengthHex(data[i], 2));
			if (i < data.length - 1) {
				sb.append(" ");
			}
		}
		sb.append("\n");

		sb.append("}\n");

		return sb.toString();
	}

}
