package net.zscript.javasimulator;

public interface CommunicationPacket<T extends ProtocolConnection<?, T>> {
    interface ResponseInfo {
    }

    Class<T> getProtocolConnectionType();

    String prettyPrint();

    CommunicationResponse<T> generateResponse(ResponseInfo info);
}
