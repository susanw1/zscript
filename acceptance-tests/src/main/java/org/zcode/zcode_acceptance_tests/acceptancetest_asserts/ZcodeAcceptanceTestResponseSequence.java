package org.zcode.zcode_acceptance_tests.acceptancetest_asserts;

import java.util.LinkedList;
import java.util.Queue;

public class ZcodeAcceptanceTestResponseSequence {
    private final Queue<ZcodeAcceptanceTestResponse> resps = new LinkedList<>();
    private boolean isBroadcast = false;
    private boolean isNotification = false;

    public void setBroadcast() {
        this.isBroadcast = true;
    }

    public boolean isEmpty() {
        return resps.isEmpty();
    }

    public void addResponse(ZcodeAcceptanceTestResponse resp) {
        resps.add(resp);
    }

    public ZcodeAcceptanceTestResponse peekFirst() {
        return resps.peek();
    }

    public ZcodeAcceptanceTestResponse pollFirst() {
        return resps.poll();
    }

    public Queue<ZcodeAcceptanceTestResponse> getResps() {
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
