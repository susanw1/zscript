package net.zscript.javasimulator.connections.i2c;

public class I2cSendResponse implements I2cResponse {
	private final int dataNackIndex;

	public static I2cSendResponse DataNack(int index) {
		return new I2cSendResponse(index);
	}

	public static I2cSendResponse Success() {
		return new I2cSendResponse(-1);
	}

	private I2cSendResponse(int dataNackIndex) {
		this.dataNackIndex = dataNackIndex;
	}

	@Override
	public Class<I2cConnection> getProtocolConnectionType() {
		return I2cConnection.class;
	}

	@Override
	public boolean worked() {
		return dataNackIndex == -1;
	}

	@Override
	public boolean addressNack() {
		return false;
	}

	public int getDataNackIndex() {
		return dataNackIndex;
	}

	@Override
	public String prettyPrint() {
		if (worked()) {
			return "";
		} else {
			return "Data Nack at position " + dataNackIndex;
		}
	}

}
