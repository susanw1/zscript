package net.zscript.javasimulator.test;

import net.zscript.javasimulator.BlankCommunicationResponse;
import net.zscript.javasimulator.CommunicationPacket;
import net.zscript.javasimulator.CommunicationResponse;
import net.zscript.javasimulator.EntityController;
import net.zscript.javasimulator.ProtocolCategory;
import net.zscript.javasimulator.ProtocolConnection;

public class BlankEntityController extends EntityController {

    public BlankEntityController() {
        super();
    }

    @Override
    public <U extends ProtocolCategory, T extends ProtocolConnection<U, T>> CommunicationResponse<T> acceptIncoming(Class<U> type, int index,
            CommunicationPacket<T> packet) {
        return new BlankCommunicationResponse<T>(packet.getProtocolConnectionType());
    }

}
