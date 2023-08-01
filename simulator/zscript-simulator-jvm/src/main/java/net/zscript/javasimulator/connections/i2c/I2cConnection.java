package net.zscript.javasimulator.connections.i2c;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import net.zscript.javasimulator.CommunicationPacket;
import net.zscript.javasimulator.CommunicationResponse;
import net.zscript.javasimulator.Connection;
import net.zscript.javasimulator.Entity;
import net.zscript.javasimulator.ProtocolConnection;

public final class I2cConnection extends ProtocolConnection<I2cProtocolCategory, I2cConnection> {
    private final Map<I2cAddress, List<Entity>> entities = new HashMap<>();

    public I2cConnection(Connection<I2cProtocolCategory> connection) {
        super(I2cProtocolCategory.class, connection);
    }

    public void connect(I2cAddress address, Entity entity) {
        entities.putIfAbsent(address, new ArrayList<>());
        entities.get(address).add(entity);
    }

    public void disconnect(I2cAddress address, Entity entity) {
        List<Entity> ents = entities.get(address);
        if (ents != null) {
            ents.remove(entity);
        }
    }

    @Override
    public CommunicationResponse<I2cConnection> sendMessage(Entity source, CommunicationPacket<I2cConnection> packet) {
        I2cAddress address = ((I2cPacket) packet).getAddress();

        List<Entity> tlist = entities.get(address);
        Entity target = tlist == null || tlist.isEmpty() ? null : tlist.get(0);
        CommunicationResponse<I2cConnection> resp;
        if (target == null) {
            resp = new I2cAddressNackResponse();
        } else {

            resp = target.receive(connection, packet);
        }
        return resp;
    }

    @Override
    public void mergeFrom(I2cConnection other) {
        for (Entry<I2cAddress, List<Entity>> entry : other.entities.entrySet()) {
            entities.putIfAbsent(entry.getKey(), new ArrayList<>());
            entities.get(entry.getKey()).addAll(entry.getValue());
        }
    }
}
