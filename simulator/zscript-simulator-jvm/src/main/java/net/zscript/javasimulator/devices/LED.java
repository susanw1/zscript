package net.zscript.javasimulator.devices;

import net.zscript.javasimulator.CommunicationPacket;
import net.zscript.javasimulator.CommunicationResponse;
import net.zscript.javasimulator.Connection;
import net.zscript.javasimulator.EntityController;
import net.zscript.javasimulator.ProtocolCategory;
import net.zscript.javasimulator.ProtocolConnection;
import net.zscript.javasimulator.connections.pin.PinConnection;
import net.zscript.javasimulator.connections.pin.PinProtocolCategory;
import net.zscript.javasimulator.connections.pin.PinStatePacket;
import net.zscript.javasimulator.connections.pin.PinWritePacket;

public class LED extends EntityController {
    private double brightness = 0;

    public LED() {
        Connection<PinProtocolCategory> line = entity.connectNew(PinProtocolCategory.class);
        PinConnection pinCon = new PinConnection(line);
        pinCon.addChild(entity);
        line.addProtocol(pinCon);
        line.sendMessage(entity, PinWritePacket.digitalDrive5V(0, 4700)); // applies a 4k7 ish pulldown.
    }

    @Override
    public <U extends ProtocolCategory, T extends ProtocolConnection<U, T>> CommunicationResponse<T> acceptIncoming(Class<U> type, int index,
            CommunicationPacket<T> packet) {
        if (packet.getClass() == PinStatePacket.class) {
            brightness = ((PinStatePacket) packet).getState() / 5;
            System.out.println("Brightness set to: " + brightness);
            return packet.generateResponse(null);
        }
        throw new IllegalArgumentException("Invalid communication for LED: " + packet.getProtocolConnectionType().getName());
    }

    public double getBrightness() {
        return brightness;
    }

}
