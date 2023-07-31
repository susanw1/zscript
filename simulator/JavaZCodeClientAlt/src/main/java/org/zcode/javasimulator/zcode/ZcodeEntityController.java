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
    private final ExecutorService mainExec = Executors.newSingleThreadExecutor();
    private final ExecutorService interruptExec = Executors.newSingleThreadExecutor();
    private final Thread mainCurrent = findCurrentMain();
    private final Thread interruptCurrent = findCurrentInterrupt();
    private final Zcode zcode = new Zcode();
    private volatile boolean hasNonWait = false;

    private Thread findCurrentMain() {
        try {
            return mainExec.submit(() -> Thread.currentThread()).get();
        } catch (Exception e) {
            throw new RuntimeException(e);
        }
    }

    private Thread findCurrentInterrupt() {
        try {
            return interruptExec.submit(() -> Thread.currentThread()).get();
        } catch (Exception e) {
            throw new RuntimeException(e);
        }
    }

    private <U> U switchToInterruptThread(Callable<U> task) {
        if (Thread.currentThread() == interruptCurrent) {
            try {
                return task.call();
            } catch (Exception e) {
                throw new RuntimeException(e);
            }
        } else {
            try {
                return interruptExec.submit(task).get();
            } catch (Throwable e) {
                throw new RuntimeException(e);
            }
        }
    }

    private void switchToInterruptThread(Runnable task) {
        if (Thread.currentThread() == interruptCurrent) {
            try {
                task.run();
            } catch (Exception e) {
                throw new RuntimeException(e);
            }
        } else {
            try {
                interruptExec.submit(task);
            } catch (Throwable e) {
                throw new RuntimeException(e);
            }
        }
    }

    private <U> U switchToMainThread(Callable<U> task) {
        if (Thread.currentThread() == mainCurrent) {
            try {
                return task.call();
            } catch (Exception e) {
                throw new RuntimeException(e);
            }
        } else {
            try {
                return mainExec.submit(task).get();
            } catch (Throwable e) {
                throw new RuntimeException(e);
            }
        }
    }

    private void switchToMainThread(Runnable task) {
        if (Thread.currentThread() == mainCurrent) {
            try {
                task.run();
            } catch (Exception e) {
                throw new RuntimeException(e);
            }
        } else {
            try {
                mainExec.submit(task);
            } catch (Throwable e) {
                throw new RuntimeException(e);
            }
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
            mainExec.submit(new ProgressForever());
        }

    }

    public void stop() {
        mainExec.shutdown();
        interruptExec.shutdown();
    }

    public boolean hasActivity() {
        return switchToMainThread(() -> hasNonWait);
    }

    public ZcodeEntityController() {
        addModule(new ZcodeCoreModule());
        addModule(new ZcodeOuterCoreModule());
        mainExec.submit(new ProgressForever());
    }

    public <U extends ProtocolCategory> void add(ZcodeSimulatorConsumer<U> consumer, int index) {
        switchToInterruptThread(() -> {
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
        switchToMainThread(() -> zcode.addChannel(channel));
    }

    public void addModule(ZcodeModule module) {
        switchToMainThread(() -> zcode.addModule(module));
    }

    @SuppressWarnings("unchecked")
    private <U extends ProtocolCategory, T extends ProtocolConnection<U, T>> ZcodeSimulatorConsumer<U> getModule(Class<T> type, int index) {
        hasNonWait = true;
        return (ZcodeSimulatorConsumer<U>) modules.get(type).get(index);
    }

    @Override
    public <U extends ProtocolCategory, T extends ProtocolConnection<U, T>> CommunicationResponse<T> acceptIncoming(Class<U> type, int index, CommunicationPacket<T> packet) {
        try {
            return switchToInterruptThread(() -> {
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
        switchToMainThread(() -> zcode.addNotificationSource(notificationSource));
    }
}
