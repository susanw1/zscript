package org.zcode.javareceiver.executionspace;

import org.zcode.javareceiver.ZcodeBusInterrupt;
import org.zcode.javareceiver.ZcodeNotificationManager;
import org.zcode.javareceiver.ZcodeParameters;

public class ZcodeInterruptVectorManager {
    private final ZcodeNotificationManager notificationManager;
    private final ZcodeExecutionSpace      space;
    private final ZcodeInterruptVectorOut  out;
    private final ZcodeInterruptVectorMap  vectorMap;
    private final ZcodeBusInterrupt[]      waitingInterrupts;
    private int                            waitingNum = 0;

    public ZcodeInterruptVectorManager(final ZcodeParameters params, final ZcodeNotificationManager notificationManager, final ZcodeExecutionSpace space) {
        this.notificationManager = notificationManager;
        this.space = space;
        this.out = new ZcodeInterruptVectorOut(notificationManager, params);
        this.vectorMap = new ZcodeInterruptVectorMap(params);
        this.waitingInterrupts = new ZcodeBusInterrupt[params.interruptVectorWorkingNum];
    }

    public ZcodeInterruptVectorMap getVectorMap() {
        return vectorMap;
    }

    public boolean canAccept() {
        return waitingNum < waitingInterrupts.length;
    }

    public void acceptInterrupt(final ZcodeBusInterrupt i) {
        waitingInterrupts[waitingNum++] = i;
    }

    public boolean hasInterruptSource() {
        return waitingNum > 0;
    }

    public ZcodeBusInterrupt takeInterrupt() {
        final ZcodeBusInterrupt interrupt = waitingInterrupts[0];
        for (int i = 0; i < waitingNum - 1; i++) {
            waitingInterrupts[i] = waitingInterrupts[i + 1];
        }
        waitingNum--;
        return interrupt;
    }

    public ZcodeExecutionSpace getSpace() {
        return space;
    }

    public boolean hasVector(final ZcodeBusInterrupt busInt) {
        return vectorMap.hasVector(busInt.getNotificationType(), busInt.getNotificationBus(), busInt.getFoundAddress(),
                busInt.getSource().hasAddress() && busInt.hasFindableAddress());

    }

    public int findVector(final ZcodeBusInterrupt busInt) {
        return vectorMap.getVector(busInt.getNotificationType(), busInt.getNotificationBus(), busInt.getFoundAddress(),
                busInt.getSource().hasAddress() && busInt.hasFindableAddress());
    }

    public ZcodeInterruptVectorOut getOut() {
        return out;
    }

    public ZcodeNotificationManager getNotificationManager() {
        return notificationManager;
    }
}
