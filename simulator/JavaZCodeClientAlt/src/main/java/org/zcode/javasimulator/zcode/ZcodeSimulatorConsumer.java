package org.zcode.javasimulator.zcode;

import org.zcode.javasimulator.CommunicationPacket;
import org.zcode.javasimulator.CommunicationResponse;
import org.zcode.javasimulator.ProtocolCategory;
import org.zcode.javasimulator.ProtocolConnection;

public interface ZcodeSimulatorConsumer<U extends ProtocolCategory> {

    <T extends ProtocolConnection<U, T>> CommunicationResponse<T> acceptPacket(int index, CommunicationPacket<T> packet);

    Class<? extends ProtocolConnection<U, ?>>[] getConnections();
}
