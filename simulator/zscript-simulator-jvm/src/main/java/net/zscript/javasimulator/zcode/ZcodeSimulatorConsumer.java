package net.zscript.javasimulator.zcode;

import net.zscript.javasimulator.CommunicationPacket;
import net.zscript.javasimulator.CommunicationResponse;
import net.zscript.javasimulator.ProtocolCategory;
import net.zscript.javasimulator.ProtocolConnection;

public interface ZcodeSimulatorConsumer<U extends ProtocolCategory> {

    <T extends ProtocolConnection<U, T>> CommunicationResponse<T> acceptPacket(int index, CommunicationPacket<T> packet);

    Class<? extends ProtocolConnection<U, ?>>[] getConnections();
}
