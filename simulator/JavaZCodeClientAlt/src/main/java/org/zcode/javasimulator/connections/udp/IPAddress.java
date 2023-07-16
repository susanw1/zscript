package org.zcode.javasimulator.connections.udp;

public class IPAddress {
	private final int addr1;
	private final int addr2;
	private final int addr3;
	private final int addr4;

	public IPAddress(int addr1, int addr2, int addr3, int addr4) {
		this.addr1 = addr1;
		this.addr2 = addr2;
		this.addr3 = addr3;
		this.addr4 = addr4;
	}

	@Override
	public int hashCode() {
		return addr1 * 12 + addr2 * 7 + addr3 * 3 + addr4;
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
		IPAddress other = (IPAddress) obj;
		return addr1 == other.addr1 && addr2 == other.addr2 && addr3 == other.addr3 && addr4 == other.addr4;
	}

	public String prettyPrint() {
		StringBuilder sb = new StringBuilder();
		sb.append(addr1);
		sb.append(".");
		sb.append(addr2);
		sb.append(".");
		sb.append(addr3);
		sb.append(".");
		sb.append(addr4);
		return sb.toString();
	}
}
