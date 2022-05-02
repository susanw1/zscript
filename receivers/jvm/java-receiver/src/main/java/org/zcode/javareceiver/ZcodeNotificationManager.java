package org.zcode.javareceiver;

import org.zcode.javareceiver.executionspace.ZcodeInterruptVectorManager;
import org.zcode.javareceiver.parsing.ZcodeCommandChannel;

public class ZcodeNotificationManager {
    private final ZcodeParameters           params;
    private final ZcodeBusInterruptSource[] sources;
    private final ZcodeBusInterrupt[]       waitingNotifications;

    private int                         waitingNotificationNumber = 0;
    private ZcodeInterruptVectorManager vectorChannel;
    private ZcodeCommandChannel         notificationChannel       = new ZcodeNoopCommandChannel();

    public ZcodeNotificationManager(final ZcodeParameters params, final ZcodeBusInterruptSource[] sources) {
        this.params = params;
        this.sources = sources;
        this.waitingNotifications = new ZcodeBusInterrupt[params.interruptStoreNum];
    }

    public void setVectorChannel(final ZcodeInterruptVectorManager vectorChannel) {
        this.vectorChannel = vectorChannel;
    }

    public void setNotificationChannel(final ZcodeCommandChannel notificationChannel) {
        if (this.notificationChannel != null) {
            this.notificationChannel.releaseFromNotificationChannel();
        }
        this.notificationChannel = notificationChannel;
        this.notificationChannel.setAsNotificationChannel();
    }

    public ZcodeCommandChannel getNotificationChannel() {
        return notificationChannel;
    }

    public ZcodeInterruptVectorManager getVectorChannel() {
        return vectorChannel;
    }

    public void manageNotifications() {
        if (waitingNotificationNumber > 0 && canSendNotification()) {
            for (int i = 0; i < waitingNotificationNumber; i++) {
                final ZcodeBusInterrupt interrupt = waitingNotifications[i];
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
            for (final ZcodeBusInterruptSource source : sources) {
                if (source.hasUncheckedNotifications()) {
                    final byte id = source.takeUncheckedNotificationId();
                    if (params.findInterruptSourceAddress && source.hasAddress()) {
                        if (source.checkFindAddressLocks(id)) {
                            source.findAddress(id);
                        }
                        if (canSendNotification() && source.hasFoundAddress(id)) {
                            sendNotification(new ZcodeBusInterrupt(source, id));
                        } else {
                            waitingNotifications[waitingNotificationNumber++] = new ZcodeBusInterrupt(source, id);
                            break;
                        }
                    } else {
                        if (canSendNotification() && (!params.findInterruptSourceAddress || source.hasFoundAddress(id))) {
                            sendNotification(new ZcodeBusInterrupt(source, id));
                        } else {
                            waitingNotifications[waitingNotificationNumber++] = new ZcodeBusInterrupt(source, id);
                            break;
                        }
                    }
                }
            }
        }
    }

    private boolean canSendNotification() {
        return (!params.isUsingInterruptVector && (!notificationChannel.hasOutStream() || !notificationChannel.acquireOutStream().isLocked()))
                || (params.isUsingInterruptVector && vectorChannel.canAccept());
    }

    private void sendNotification(final ZcodeBusInterrupt interrupt) {
        if (params.isUsingInterruptVector && vectorChannel.hasVector(interrupt)) {
            vectorChannel.acceptInterrupt(interrupt);
        } else {
            final ZcodeOutStream out = notificationChannel.acquireOutStream();
            if (out.isOpen()) {
                out.close();
                out.mostRecent = interrupt.getSource();
            }
            out.openNotification(notificationChannel);
            out.markNotification();
            out.writeField(Zchars.NOTIFY_TYPE_PARAM.ch, (byte) 1);
            out.writeField('A', (byte) 1);
            out.writeField('T', interrupt.getNotificationType());
            out.writeField('I', interrupt.getNotificationBus());
            out.writeStatus(ZcodeResponseStatus.OK);
            if (params.findInterruptSourceAddress && interrupt.getSource().hasAddress()) {
                out.writeCommandSeparator();
                out.writeField('A', interrupt.getFoundAddress());
                if (interrupt.hasFindableAddress()) {
                    out.writeStatus(ZcodeResponseStatus.OK);
                } else {
                    out.writeStatus(ZcodeResponseStatus.CMD_FAIL);
                }
            }
            out.writeCommandSequenceSeparator();
            out.close();
            notificationChannel.releaseOutStream();
            interrupt.clear();
        }
    }
}
