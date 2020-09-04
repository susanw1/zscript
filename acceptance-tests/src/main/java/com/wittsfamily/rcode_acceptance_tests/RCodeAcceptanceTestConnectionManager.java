package com.wittsfamily.rcode_acceptance_tests;

import java.util.ArrayList;
import java.util.List;

import com.wittsfamily.rcode_acceptance_tests.acceptancetest_asserts.RCodeAcceptanceTestConnection;

public class RCodeAcceptanceTestConnectionManager {
    private static List<RCodeAcceptanceTestConnection> connections = new ArrayList<>();

    public static void registerConnection(RCodeAcceptanceTestConnection con) {
        connections.add(con);
    }

    public static List<RCodeAcceptanceTestConnection> getConnections() {
        return connections;
    }
}
