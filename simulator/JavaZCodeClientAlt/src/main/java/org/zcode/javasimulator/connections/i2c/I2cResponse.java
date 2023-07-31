package org.zcode.javasimulator.connections.i2c;

import org.zcode.javasimulator.CommunicationResponse;

public interface I2cResponse extends CommunicationResponse<I2cConnection> {

	public boolean worked();

	public boolean addressNack();
}
