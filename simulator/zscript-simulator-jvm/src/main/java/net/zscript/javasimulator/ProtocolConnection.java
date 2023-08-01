package net.zscript.javasimulator;

public abstract class ProtocolConnection<T extends ProtocolCategory, U extends ProtocolConnection<T, U>> {
    private final Class<T> protocolCategory;
    protected final Connection<T> connection;

    public ProtocolConnection(Class<T> protocolCategory, Connection<T> connection) {
        this.protocolCategory = protocolCategory;
        this.connection = connection;
    }

    public Class<T> getProtocolCategory() {
        return protocolCategory;
    }

    public abstract CommunicationResponse<U> sendMessage(Entity source, CommunicationPacket<U> packet);

    public abstract void mergeFrom(U other);
}
