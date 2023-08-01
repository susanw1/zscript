package net.zscript.javasimulator.connections.i2c;

import net.zscript.javasimulator.CommunicationResponse;

public interface I2cResponse extends CommunicationResponse<I2cConnection> {

	public boolean worked();

	public boolean addressNack();
}
