package org.zcode.javasimulator.zcode;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.Callable;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import org.zcode.javareceiver.core.Zcode;
import org.zcode.javareceiver.core.ZcodeChannel;
import org.zcode.javareceiver.modules.ZcodeModule;
import org.zcode.javareceiver.modules.core.ZcodeCoreModule;
import org.zcode.javareceiver.modules.outerCore.ZcodeOuterCoreModule;
import org.zcode.javareceiver.notifications.ZcodeNotificationSource;
import org.zcode.javasimulator.BlankCommunicationResponse;
import org.zcode.javasimulator.CommunicationPacket;
import org.zcode.javasimulator.CommunicationResponse;
import org.zcode.javasimulator.EntityController;
import org.zcode.javasimulator.ProtocolCategory;
import org.zcode.javasimulator.ProtocolConnection;

public class ZcodeEntityController extends EntityController {
    private final Map<Class<? extends ProtocolConnection<?, ?>>, List<ZcodeSimulatorConsumer<? extends ProtocolCategory>>> modules = new HashMap<>();
    private final Zcode zcode = new Zcode();
    private final ExecutorService exec = Executors.newSingleThreadExecutor();
    private final Thread current = findCurrent();
    private volatile boolean hasNonWait = false;

    private Thread findCurrent() {
        try {
            return exec.submit(() -> Thread.currentThread()).get();
        } catch (Exception e) {
            throw new RuntimeException(e);
        }
    }

    private <U> U switchToThread(Callable<U> task) {
        if (Thread.currentThread() == current) {
            try {
                return task.call();
            } catch (Exception e) {
                throw new RuntimeException(e);
            }
        } else {
            try {
                return exec.submit(task).get();
            } catch (Throwable e) {
                throw new RuntimeException(e);
            }
        }
    }

    private void switchToThread(Runnable task) {
        if (Thread.currentThread() == current) {
            try {
                task.run();
            } catch (Throwable t) {
                throw new RuntimeException(t);
            }
        } else {
            exec.submit(task);
        }
    }

    private class ProgressForever implements Runnable {

        @Override
        public void run() {
            try {
                hasNonWait = zcode.progress();
            } catch (Exception e) {
                hasNonWait = false;
                e.printStackTrace();
            }
            exec.submit(new ProgressForever());
        }

    }

    public void stop() {
        exec.shutdown();
    }

    public boolean hasActivity() {
        return switchToThread(() -> hasNonWait);
    }

    public ZcodeEntityController() {
        addModule(new ZcodeCoreModule());
        addModule(new ZcodeOuterCoreModule());
        exec.submit(new ProgressForever());
    }

    public <U extends ProtocolCategory> void add(ZcodeSimulatorConsumer<U> consumer, int index) {
        switchToThread(() -> {
            for (Class<? extends ProtocolConnection<U, ?>> connection : consumer.getConnections()) {
                modules.putIfAbsent(connection, new ArrayList<>());
                List<ZcodeSimulatorConsumer<? extends ProtocolCategory>> list = modules.get(connection);
                while (list.size() <= index) {
                    list.add(null);
                }
                list.set(index, consumer);
            }
        });
    }

    public void addChannel(ZcodeChannel channel) {
        switchToThread(() -> zcode.addChannel(channel));
    }

    public void addModule(ZcodeModule module) {
        switchToThread(() -> zcode.addModule(module));
    }

    public <U extends ProtocolCategory, T extends ProtocolConnection<U, T>> ZcodeSimulatorConsumer<U> getModule(Class<T> type, int index) {
        try {
            return switchToThread(() -> {
                hasNonWait = true;
                return (ZcodeSimulatorConsumer<U>) modules.get(type).get(index);
            });
        } catch (Exception e) {
            throw new RuntimeException(e);
        }
    }

    @Override
    public <U extends ProtocolCategory, T extends ProtocolConnection<U, T>> CommunicationResponse<T> acceptIncoming(Class<U> type, int index, CommunicationPacket<T> packet) {
        try {
            return switchToThread(() -> {
                ZcodeSimulatorConsumer<U> consumer = getModule(packet.getProtocolConnectionType(), index);

                if (consumer == null) {
                    return new BlankCommunicationResponse<>(packet.getProtocolConnectionType());
                } else {
                    return consumer.acceptPacket(index, packet);
                }
            });
        } catch (Exception e) {
            throw new RuntimeException(e);
        }
    }

    public void add(ZcodeNotificationSource notificationSource) {
        switchToThread(() -> zcode.addNotificationSource(notificationSource));
    }
}
