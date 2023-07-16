package org.zcode.javasimulator.connections.i2c;

import org.zcode.javasimulator.CommunicationPacket;

public class I2cReceivePacket implements CommunicationPacket<I2cConnection> {
	private final I2cAddress address;
	private final long baud;
	private final boolean hasStopBit;

	private final int length;

	public I2cReceivePacket(I2cAddress address, int length) {
		this(address, 0, true, length);
	}

	public I2cReceivePacket(I2cAddress address, long baud, boolean hasStopBit, int length) {
		this.address = address;
		this.baud = baud;
		this.hasStopBit = hasStopBit;
		this.length = length;
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

	public int getLength() {
		return length;
	}

	@Override
	public String prettyPrint() {
		StringBuilder sb = new StringBuilder();
		sb.append("{\n");
		sb.append("\tType: I2C receive\n");

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

		sb.append("\tLength: ");
		sb.append(length);
		sb.append("\n");

		if (!hasStopBit) {
			sb.append("\tNo stop");
			sb.append("\n");
		}

		sb.append("}\n");

		return sb.toString();
	}

}
