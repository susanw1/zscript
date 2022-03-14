package org.zcode.zcode_acceptance_tests;

import java.util.ArrayList;
import java.util.List;

import org.zcode.zcode_acceptance_tests.acceptancetest_asserts.ZcodeAcceptanceTestConnection;

public class ZcodeAcceptanceTestConnectionManager {
    private static List<ZcodeAcceptanceTestConnection> connections = new ArrayList<>();

    public static void registerConnection(ZcodeAcceptanceTestConnection con) {
        connections.add(con);
    }

    public static List<ZcodeAcceptanceTestConnection> getConnections() {
        return connections;
    }
}
