package org.zcode.javasimulator;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class Entity {
    private class ConnectionMultiMap {
        private final Map<Class<? extends ProtocolCategory>, List<Connection<?>>> connections = new HashMap<>();

        public void add(Connection<?> connection) {
            List<Connection<?>> cons = connections.get(connection.getProtocolCategory());
            if (cons == null) {
                cons = new ArrayList<>();
                connections.put(connection.getProtocolCategory(), cons);
            }
            cons.add(connection);
        }

        public void set(int i, Connection<?> connection) {
            List<Connection<?>> cons = connections.get(connection.getProtocolCategory());
            if (cons == null) {
                cons = new ArrayList<>();
                connections.put(connection.getProtocolCategory(), cons);
            }
            cons.set(i, connection);
        }

        public <T extends ProtocolCategory> List<Connection<T>> get(Class<T> category) {
            return messTypes(connections.getOrDefault(category, List.of()));
        }

        public <T extends ProtocolCategory> void replace(Class<T> category, Connection<T> connection, Connection<T> replacement) {
            List<Connection<T>> cons = get(category);
            for (int i = 0; i < cons.size(); i++) {
                if (cons.get(i) == connection) {
                    cons.set(i, replacement);
                    return;
                }
            }
        }

        @SuppressWarnings("unchecked")
        private <T extends ProtocolCategory> List<Connection<T>> messTypes(List<?> list) {
            return (List<Connection<T>>) list;
        }

    }

    private final ConnectionMultiMap map = new ConnectionMultiMap();
    private final EntityController controller;

    public Entity(EntityController controller) {
        this.controller = controller;
    }

    public <T extends ProtocolCategory> Connection<T> getConnection(Class<T> connectionCategory, int index) {
        return map.get(connectionCategory).get(index);
    }

    public <T extends ProtocolCategory> int countConnection(Class<T> connectionCategory) {
        return map.get(connectionCategory).size();
    }

    public <U extends ProtocolCategory, T extends ProtocolConnection<U, T>> CommunicationResponse<T> receive(Connection<U> connection,
            CommunicationPacket<T> packet) {
        List<Connection<U>> possibilities = map.get(connection.getProtocolCategory());
        if (possibilities == null) {
            throw new IllegalArgumentException("Entity does not have connection of type " + connection.getClass().getName());
        }
        for (int i = 0; i < possibilities.size(); i++) {
            if (possibilities.get(i) == connection) {
                return controller.acceptIncoming(connection.getProtocolCategory(), i, packet);
            }
        }
        throw new IllegalArgumentException("Entity received message from connection it isn't connected to");
    }

    public <U extends ProtocolCategory> Connection<U> connectNew(Class<U> category) {
        Connection<U> con = new Connection<>(category);
        map.add(con);
        con.addEntity(this);
        return con;
    }

    public <U extends ProtocolCategory> void changeConnection(Class<U> category, Connection<U> old, Connection<U> replacement) {
        map.replace(category, old, replacement);
    }

    public <U extends ProtocolCategory> void connect(Class<U> category, int localIndex, Entity other, int otherIndex) {
        Connection<U> source = other.map.get(category).get(otherIndex);
        Connection<U> dest = map.get(category).get(localIndex);
        dest.mergeConnection(source);
    }
}
