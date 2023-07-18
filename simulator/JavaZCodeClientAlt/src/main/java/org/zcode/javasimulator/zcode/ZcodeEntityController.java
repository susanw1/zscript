package org.zcode.javasimulator.zcode;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import org.zcode.javareceiver.core.Zcode;
import org.zcode.javareceiver.core.ZcodeChannel;
import org.zcode.javareceiver.modules.ZcodeCommandFinder;
import org.zcode.javareceiver.modules.ZcodeModule;
import org.zcode.javareceiver.modules.core.ZcodeCoreModule;
import org.zcode.javareceiver.modules.outerCore.ZcodeOuterCoreModule;
import org.zcode.javasimulator.BlankCommunicationResponse;
import org.zcode.javasimulator.CommunicationPacket;
import org.zcode.javasimulator.CommunicationResponse;
import org.zcode.javasimulator.EntityController;
import org.zcode.javasimulator.ProtocolCategory;
import org.zcode.javasimulator.ProtocolConnection;

public class ZcodeEntityController extends EntityController {
    private final Map<Class<? extends ProtocolConnection<?, ?>>, List<ZcodeSimulatorConsumer<? extends ProtocolCategory>>> modules = new HashMap<>();
    private final Zcode zcode = Zcode.getZcode();
    private final ExecutorService exec = Executors.newSingleThreadExecutor();

    private class ProgressForever implements Runnable {

        @Override
        public void run() {
            try {
                zcode.progress();
                Thread.sleep(10);
            } catch (Exception e) {
                e.printStackTrace();
            }
            exec.submit(this);
        }

    }

    public ZcodeEntityController() {
        addModule(new ZcodeCoreModule());
        addModule(new ZcodeOuterCoreModule());
        exec.submit(new ProgressForever());
    }

    public <U extends ProtocolCategory> void add(ZcodeSimulatorConsumer<U> consumer, int index) {
        for (Class<? extends ProtocolConnection<U, ?>> connection : consumer.getConnections()) {
            modules.putIfAbsent(connection, new ArrayList<>());
            List<ZcodeSimulatorConsumer<? extends ProtocolCategory>> list = modules.get(connection);
            while (list.size() <= index) {
                list.add(null);
            }
            list.set(index, consumer);
        }
    }

    public void addChannel(ZcodeChannel channel) {
        zcode.addChannel(channel);
    }

    public void addModule(ZcodeModule module) {
        ZcodeCommandFinder.addModule(module);
    }

    public <U extends ProtocolCategory, T extends ProtocolConnection<U, T>> ZcodeSimulatorConsumer<U> getModule(Class<T> type, int index) {
        return (ZcodeSimulatorConsumer<U>) modules.get(type).get(index);
    }

    @Override
    public <U extends ProtocolCategory, T extends ProtocolConnection<U, T>> CommunicationResponse<T> acceptIncoming(Class<U> type, int index, CommunicationPacket<T> packet) {
        ZcodeSimulatorConsumer<U> consumer = getModule(packet.getProtocolConnectionType(), index);
        if (consumer == null) {
            return new BlankCommunicationResponse<>(packet.getProtocolConnectionType());
        } else {
            return consumer.acceptPacket(index, packet);
        }
    }
}
