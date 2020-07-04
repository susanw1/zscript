package com.wittsfamily.rcode.executionspace;

import com.wittsfamily.rcode.RCodeBusInterrupt;
import com.wittsfamily.rcode.RCodeNotificationManager;
import com.wittsfamily.rcode.RCodeParameters;

public class RCodeInterruptVectorManager {
    private final RCodeNotificationManager notificationManager;
    private final RCodeExecutionSpace space;
    private final RCodeInterruptVectorOut out;
    private RCodeBusInterrupt[] waitingInterrupts;
    private int waitingNum = 0;

    private RCodeInterruptVectorManager(RCodeParameters params, RCodeNotificationManager notificationManager, RCodeExecutionSpace space) {
        this.notificationManager = notificationManager;
        this.space = space;
        this.out = new RCodeInterruptVectorOut(notificationManager, params);
        this.waitingInterrupts = new RCodeBusInterrupt[params.interruptVectorWorkingNum];
    }

    public boolean canAccept() {
        return waitingNum < waitingInterrupts.length;
    }

    public void acceptInterrupt(RCodeBusInterrupt i) {
        waitingInterrupts[waitingNum++] = i;
    }

    public boolean hasInterruptSource() {
        return waitingNum > 0;
    }

    public RCodeBusInterrupt takeInterrupt() {
        RCodeBusInterrupt interrupt = waitingInterrupts[0];
        for (int i = 0; i < waitingNum - 1; i++) {
            waitingInterrupts[i] = waitingInterrupts[i + 1];
        }
        return interrupt;
    }

    public RCodeExecutionSpace getSpace() {
        return space;
    }

    public int findVector(RCodeBusInterrupt busInt) {

    }

    public RCodeInterruptVectorOut getOut() {
        return out;
    }

    public RCodeNotificationManager getNotificationManager() {
        return notificationManager;
    }
}
