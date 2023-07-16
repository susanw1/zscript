package org.zcode.javasimulator.connections.i2c;

public class I2cAddress {
	private final boolean tenBit;
	private final int address;

	public I2cAddress(int address) {
		this(false, address);
	}

	public I2cAddress(boolean tenBit, int address) {
		if (address < 0 || address > 0x3FF) {
			throw new IllegalArgumentException("Invalid I2C address: " + address);
		} else if (!tenBit && address > 0x7F) {
			throw new IllegalArgumentException("Invalid 7 bit I2C address: " + address);
		}
		this.tenBit = tenBit;
		this.address = address;
	}

	public boolean isTenBit() {
		return tenBit;
	}

	public int getAddress() {
		return address;
	}

	@Override
	public int hashCode() {
		return address + (tenBit ? 1000000 : 0);
	}

	@Override
	public boolean equals(Object obj) {
		if (this == obj) {
			return true;
		}
		if (obj == null) {
			return false;
		}
		if (getClass() != obj.getClass()) {
			return false;
		}
		I2cAddress other = (I2cAddress) obj;
		return address == other.address && tenBit == other.tenBit;
	}

	public String prettyPrint() {
		String s = Integer.toUnsignedString(address, 16);
		if (tenBit) {
			s += "(10 bit)";
		}
		return s;
	}

}
