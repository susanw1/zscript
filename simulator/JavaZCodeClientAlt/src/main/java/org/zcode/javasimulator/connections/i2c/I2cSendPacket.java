package org.zcode.javasimulator.connections.i2c;

import org.zcode.javasimulator.CommunicationPacket;
import org.zcode.javasimulator.Utility;

public class I2cSendPacket implements CommunicationPacket<I2cConnection> {
	private final I2cAddress address;
	private final long baud;
	private final boolean hasStopBit;

	private final byte[] data;

	public I2cSendPacket(I2cAddress address, byte[] data) {
		this(address, 0, true, data);
	}

	public I2cSendPacket(I2cAddress address, long baud, boolean hasStopBit, byte[] data) {
		this.address = address;
		this.baud = baud;
		this.hasStopBit = hasStopBit;
		this.data = data;
	}

	@Override
	public Class<I2cConnection> getProtocolConnectionType() {
		return I2cConnection.class;
	}

	public I2cAddress getAddress() {
		return address;
	}

	public long getBaud() {
		return baud;
	}

	public boolean isHasStopBit() {
		return hasStopBit;
	}

	public byte[] getData() {
		return data;
	}

	@Override
	public String prettyPrint() {
		StringBuilder sb = new StringBuilder();
		sb.append("{\n");
		sb.append("\tType: I2C send\n");
		if (baud != 0) {
			sb.append("\tBaud rate: ");
			if (baud % 1000000 == 0) {
				sb.append(baud / 1000000);
				sb.append("MHz");
			} else if (baud % 1000 == 0) {
				sb.append(baud / 1000);
				sb.append("kHz");
			} else {
				sb.append(baud);
				sb.append("Hz");
			}
			sb.append("\n");
		}

		sb.append("\tAddress: ");
		sb.append(address.prettyPrint());
		sb.append("\n");

		sb.append("\tData: ");
		for (int i = 0; i < data.length; i++) {
			sb.append(Utility.toFixedLengthHex(data[i], 2));
			if (i < data.length - 1) {
				sb.append(" ");
			}
		}
		sb.append("\n");

		if (!hasStopBit) {
			sb.append("\tNo stop");
			sb.append("\n");
		}

		sb.append("}\n");

		return sb.toString();
	}

}
