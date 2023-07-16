package org.zcode.javasimulator.test;

import org.zcode.javasimulator.BlankCommunicationResponse;
import org.zcode.javasimulator.CommunicationPacket;
import org.zcode.javasimulator.CommunicationResponse;
import org.zcode.javasimulator.EntityController;
import org.zcode.javasimulator.ProtocolCategory;
import org.zcode.javasimulator.ProtocolConnection;

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
