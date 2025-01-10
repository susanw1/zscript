package net.zscript.zscript_acceptance_tests.acceptancetest_asserts;

import java.util.LinkedList;
import java.util.Queue;

public class AcceptanceTestResponseSequence {
    private final Queue<AcceptanceTestResponse> resps = new LinkedList<>();
    private boolean isBroadcast = false;
    private boolean isNotification = false;

    public void setBroadcast() {
        this.isBroadcast = true;
    }

    public boolean isEmpty() {
        return resps.isEmpty();
    }

    public void addResponse(AcceptanceTestResponse resp) {
        resps.add(resp);
    }

    public AcceptanceTestResponse peekFirst() {
        return resps.peek();
    }

    public AcceptanceTestResponse pollFirst() {
        return resps.poll();
    }

    public Queue<AcceptanceTestResponse> getResps() {
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
