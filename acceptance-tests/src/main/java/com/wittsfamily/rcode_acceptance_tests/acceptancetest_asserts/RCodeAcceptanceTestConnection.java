package com.wittsfamily.rcode_acceptance_tests.acceptancetest_asserts;

import java.util.function.Consumer;

public interface RCodeAcceptanceTestConnection {
    public void send(byte[] message);

    public void registerListener(Consumer<byte[]> handler);

    public void clearListeners();

}
