package net.zscript.javasimulator.connections.i2c;

public class I2cAddressNackResponse implements I2cResponse {

	@Override
	public Class<I2cConnection> getProtocolConnectionType() {
		return I2cConnection.class;
	}

	@Override
	public String prettyPrint() {
		return "Address Nack";
	}

	@Override
	public boolean worked() {
		return false;
	}

	@Override
	public boolean addressNack() {
		return true;
	}

}
