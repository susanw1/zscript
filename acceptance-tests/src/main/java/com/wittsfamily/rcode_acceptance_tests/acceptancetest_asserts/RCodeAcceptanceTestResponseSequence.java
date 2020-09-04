package com.wittsfamily.rcode_acceptance_tests.acceptancetest_asserts;

import java.util.LinkedList;
import java.util.Queue;

public class RCodeAcceptanceTestResponseSequence {
    private final Queue<RCodeAcceptanceTestResponse> resps = new LinkedList<>();
    private boolean isBroadcast = false;
    private boolean isNotification = false;

    public void setBroadcast() {
        this.isBroadcast = true;
    }

    public boolean isEmpty() {
        return resps.isEmpty();
    }

    public void addResponse(RCodeAcceptanceTestResponse resp) {
        resps.add(resp);
    }

    public RCodeAcceptanceTestResponse peekFirst() {
        return resps.peek();
    }

    public RCodeAcceptanceTestResponse pollFirst() {
        return resps.poll();
    }

    public Queue<RCodeAcceptanceTestResponse> getResps() {
        return resps;
    }

    public boolean isBroadcast() {
        return isBroadcast;
    }

    public boolean isNotification() {
        return isNotification;
    }

    public void setNotification() {
        isNotification = true;
    }
}
