package net.zscript.javasimulator.test;

import net.zscript.javasimulator.BlankCommunicationResponse;
import net.zscript.javasimulator.CommunicationPacket;
import net.zscript.javasimulator.CommunicationResponse;
import net.zscript.javasimulator.Connection;
import net.zscript.javasimulator.EntityController;
import net.zscript.javasimulator.ProtocolCategory;
import net.zscript.javasimulator.ProtocolConnection;
import net.zscript.javasimulator.connections.i2c.I2cAddress;
import net.zscript.javasimulator.connections.i2c.I2cConnection;
import net.zscript.javasimulator.connections.i2c.I2cProtocolCategory;
import net.zscript.javasimulator.connections.i2c.I2cSendPacket;

public class TestEntityController extends EntityController {

    public TestEntityController() {
        Connection<I2cProtocolCategory> connection = entity.connectNew(I2cProtocolCategory.class);
        connection.addProtocol(new I2cConnection(connection));
    }

    public void sendTest() {
        Connection<I2cProtocolCategory> connection = entity.getConnection(I2cProtocolCategory.class, 0);
        connection.sendMessage(entity, new I2cSendPacket(new I2cAddress(0x23), new byte[] { 0x22, 0x44 }));
    }

    @Override
    public <U extends ProtocolCategory, T extends ProtocolConnection<U, T>> CommunicationResponse<T> acceptIncoming(Class<U> type, int index,
            CommunicationPacket<T> packet) {
        if (packet.getProtocolConnectionType() == I2cConnection.class) {
            return packet.generateResponse(((I2cSendPacket) packet).dataNack(1));
        }
        return new BlankCommunicationResponse<T>(packet.getProtocolConnectionType());
    }

}
