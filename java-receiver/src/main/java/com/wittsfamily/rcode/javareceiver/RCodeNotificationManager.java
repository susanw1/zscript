package com.wittsfamily.rcode.javareceiver;

import com.wittsfamily.rcode.javareceiver.executionspace.RCodeInterruptVectorManager;
import com.wittsfamily.rcode.javareceiver.parsing.RCodeCommandChannel;

public class RCodeNotificationManager {
    private final RCodeParameters params;
    private final RCodeBusInterruptSource[] sources;
    private final RCodeBusInterrupt[] waitingNotifications;
    private int waitingNotificationNumber = 0;
    private RCodeInterruptVectorManager vectorChannel;
    private RCodeCommandChannel notificationChannel = new RCodeVoidCommandChannel();

    public RCodeNotificationManager(RCodeParameters params, RCodeBusInterruptSource[] sources) {
        this.params = params;
        this.sources = sources;
        waitingNotifications = new RCodeBusInterrupt[params.interruptStoreNum];
    }

    public void setVectorChannel(RCodeInterruptVectorManager vectorChannel) {
        this.vectorChannel = vectorChannel;
    }

    public void setNotificationChannel(RCodeCommandChannel notificationChannel) {
        if (this.notificationChannel != null) {
            this.notificationChannel.releaseFromNotificationChannel();
        }
        this.notificationChannel = notificationChannel;
        this.notificationChannel.setAsNotificationChannel();
    }

    public RCodeCommandChannel getNotificationChannel() {
        return notificationChannel;
    }

    public RCodeInterruptVectorManager getVectorChannel() {
        return vectorChannel;
    }

    public void manageNotifications() {
        if (waitingNotificationNumber > 0 && canSendNotification()) {
            for (int i = 0; i < waitingNotificationNumber; i++) {
                RCodeBusInterrupt interrupt = waitingNotifications[i];
                if (params.findInterruptSourceAddress && !interrupt.hasStartedAddressFind()) {
                    if (interrupt.checkFindAddressLocks()) {
                        interrupt.findAddress();
                    }
                }
                if (!params.findInterruptSourceAddress || interrupt.hasFoundAddress()) {
                    sendNotification(interrupt);
                    for (int j = i; j < waitingNotificationNumber - 1; j++) {
                        waitingNotifications[j] = waitingNotifications[j + 1];
                    }
                    waitingNotificationNumber--;
                    i--;
                }
            }
        }
        if (waitingNotificationNumber < waitingNotifications.length) {
            for (RCodeBusInterruptSource source : sources) {
                if (source.hasUncheckedNotifications()) {
                    byte id = source.takeUncheckedNotificationId();
                    if (params.findInterruptSourceAddress && source.hasAddress()) {
                        if (source.checkFindAddressLocks(id)) {
                            source.findAddress(id);
                        }
                        if (canSendNotification() && source.hasFoundAddress(id)) {
                            sendNotification(new RCodeBusInterrupt(source, id));
                        } else {
                            waitingNotifications[waitingNotificationNumber++] = new RCodeBusInterrupt(source, id);
                            break;
                        }
                    } else {
                        if (canSendNotification() && (!params.findInterruptSourceAddress || source.hasFoundAddress(id))) {
                            sendNotification(new RCodeBusInterrupt(source, id));
                        } else {
                            waitingNotifications[waitingNotificationNumber++] = new RCodeBusInterrupt(source, id);
                            break;
                        }
                    }
                }
            }
        }
    }

    private boolean canSendNotification() {
        return (!params.isUsingInterruptVector && !notificationChannel.getOutStream().isLocked()) || (params.isUsingInterruptVector && vectorChannel.canAccept());
    }

    private void sendNotification(RCodeBusInterrupt interrupt) {
        if (params.isUsingInterruptVector && vectorChannel.hasVector(interrupt)) {
            vectorChannel.acceptInterrupt(interrupt);
        } else {
            RCodeOutStream out = notificationChannel.getOutStream();
            if (out.isOpen()) {
                out.close();
                out.mostRecent = interrupt.getSource();
            }
            out.openNotification(notificationChannel);
            out.markNotification();
            out.writeField('Z', (byte) 1);
            out.writeField('A', (byte) 1);
            out.writeField('T', interrupt.getNotificationType());
            out.writeField('I', interrupt.getNotificationBus());
            out.writeStatus(RCodeResponseStatus.OK);
            if (params.findInterruptSourceAddress && interrupt.getSource().hasAddress()) {
                out.writeCommandSeperator();
                out.writeField('A', interrupt.getFoundAddress());
                if (interrupt.hasFindableAddress()) {
                    out.writeStatus(RCodeResponseStatus.OK);
                } else {
                    out.writeStatus(RCodeResponseStatus.CMD_FAIL);
                }
            }
            out.writeCommandSequenceSeperator();
            out.close();
            interrupt.clear();
        }
    }
}
