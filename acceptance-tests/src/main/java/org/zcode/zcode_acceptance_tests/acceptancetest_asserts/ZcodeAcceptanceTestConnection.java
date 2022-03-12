package org.zcode.zcode_acceptance_tests.acceptancetest_asserts;

import java.util.function.Consumer;

public interface ZcodeAcceptanceTestConnection {
    public void send(byte[] message);

    public void registerListener(Consumer<byte[]> handler);

    public void clearListeners();

}
