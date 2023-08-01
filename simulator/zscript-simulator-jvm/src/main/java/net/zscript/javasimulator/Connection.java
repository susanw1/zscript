package net.zscript.javasimulator;

import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;

public final class Connection<T extends ProtocolCategory> {
    private final Map<Class<? extends ProtocolConnection<T, ?>>, ProtocolConnection<T, ?>> protocols = new HashMap<>();
    private final Set<Entity> connected = new HashSet<>();
    private final Class<T> protocolCategory;
    private ConnectionTap tap = null;

    public Connection(Class<T> protocolCategory) {
        this.protocolCategory = protocolCategory;
    }

    public Class<T> getProtocolCategory() {
        return protocolCategory;
    }

    @SuppressWarnings("unchecked")
    public void addProtocol(ProtocolConnection<T, ?> p) {
        protocols.put((Class<? extends ProtocolConnection<T, ?>>) p.getClass(), p);
    }

    @SuppressWarnings("unchecked")
    public <U extends ProtocolConnection<T, U>> U getProtocol(Class<U> key) {
        return (U) protocols.get(key);
    }

    public <U extends ProtocolConnection<T, U>> CommunicationResponse<U> sendMessage(Entity source, CommunicationPacket<U> packet) {
        CommunicationResponse<U> resp = getProtocol(packet.getProtocolConnectionType()).sendMessage(source, packet);
        messagePassed(packet, resp);
        return resp;
    }

    public void addEntity(Entity e) {
        connected.add(e);
    }

    public void tapConnection(ConnectionTap tap) {
        this.tap = tap;
    }

    public void untapConnection() {
        tap = null;
    }

    private void messagePassed(CommunicationPacket<?> packet, CommunicationResponse<?> response) {
        if (tap != null) {
            tap.messagePassed(this, packet, response);
        }
    }

    public void mergeConnection(Connection<T> other) {
        if (other.protocolCategory != protocolCategory) {
            throw new IllegalStateException("Something went badly wrong with generics.");
        }
        if (tap == null) {
            tap = other.tap;
        }
        for (ProtocolConnection<T, ?> connection : other.protocols.values()) {
            ProtocolConnection<T, ?> current = protocols.get(connection.getClass());
            if (current == null) {
                addProtocol(connection);
            } else {
                mergeProtocols(current, connection);
            }
        }
        for (Entity entity : other.connected) {
            entity.changeConnection(protocolCategory, other, this);
        }
    }

    @SuppressWarnings("unchecked")
    private <U extends ProtocolConnection<T, U>> void mergeProtocols(ProtocolConnection<T, U> current, ProtocolConnection<T, ?> connection) {
        if (current.getClass() != connection.getClass()) {
            throw new IllegalStateException("Something went badly wrong with generics.");
        }
        current.mergeFrom((U) connection);
    }

}
