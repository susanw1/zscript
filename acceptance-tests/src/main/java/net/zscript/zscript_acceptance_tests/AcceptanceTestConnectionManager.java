package net.zscript.zscript_acceptance_tests;

import java.util.ArrayList;
import java.util.List;

import net.zscript.zscript_acceptance_tests.acceptancetest_asserts.AcceptanceTestConnection;

public class AcceptanceTestConnectionManager {
    private static List<AcceptanceTestConnection> connections = new ArrayList<>();

    public static void registerConnection(AcceptanceTestConnection con) {
        connections.add(con);
    }

    public static List<AcceptanceTestConnection> getConnections() {
        return connections;
    }
}
