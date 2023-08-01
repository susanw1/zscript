package net.zscript.javasimulator.connections.spi;

public class SpiAddress {
	private final int address;

	public SpiAddress(int address) {
		this.address = address;
	}

	public int getAddress() {
		return address;
	}

	@Override
	public int hashCode() {
		return address;
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
		SpiAddress other = (SpiAddress) obj;
		return address == other.address;
	}

	public String prettyPrint() {
		String s = Integer.toHexString(address);
		return s;
	}
}
