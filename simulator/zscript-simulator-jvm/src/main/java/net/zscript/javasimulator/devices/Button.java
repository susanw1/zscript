package net.zscript.javasimulator.devices;

import net.zscript.javasimulator.CommunicationPacket;
import net.zscript.javasimulator.CommunicationResponse;
import net.zscript.javasimulator.Connection;
import net.zscript.javasimulator.EntityController;
import net.zscript.javasimulator.ProtocolCategory;
import net.zscript.javasimulator.ProtocolConnection;
import net.zscript.javasimulator.connections.pin.PinConnection;
import net.zscript.javasimulator.connections.pin.PinProtocolCategory;
import net.zscript.javasimulator.connections.pin.PinWritePacket;

public class Button extends EntityController {

    public Button() {
        Connection<PinProtocolCategory> line = entity.connectNew(PinProtocolCategory.class);
        PinConnection pinCon = new PinConnection(line);
        pinCon.addChild(entity);
        line.addProtocol(pinCon);
        release();
    }

    @Override
    public <U extends ProtocolCategory, T extends ProtocolConnection<U, T>> CommunicationResponse<T> acceptIncoming(Class<U> type, int index,
            CommunicationPacket<T> packet) {
        if (packet.getProtocolConnectionType() == PinConnection.class) {
            return packet.generateResponse(null);
        }
        throw new IllegalArgumentException("Invalid communication for LED: " + packet.getProtocolConnectionType().getName());
    }

    public void press() {
        entity.getConnection(PinProtocolCategory.class, 0).sendMessage(entity, PinWritePacket.digitalDrive5V(true));
    }

    public void release() {
        entity.getConnection(PinProtocolCategory.class, 0).sendMessage(entity, PinWritePacket.digitalDrive5V(false));
    }
}
