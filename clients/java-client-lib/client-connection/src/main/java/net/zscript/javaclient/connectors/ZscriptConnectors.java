package net.zscript.javaclient.connectors;

import java.util.Collection;
import java.util.List;
import java.util.Map;
import java.util.Optional;
import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;

import net.zscript.javaclient.connection.ZscriptConnection;

public class ZscriptConnectors {
    private static Map<String, Connector<?>> registeredConnectors = new ConcurrentHashMap<>();

    public static Collection<Connector<?>> getAvailableConnectors() {
        return registeredConnectors.values();
    }

    public static Optional<Connector<?>> getConnector(final String name) {
        return Optional.ofNullable(registeredConnectors.get(name));
    }

    protected static void register(Connector<?> connector) {
        registeredConnectors.putIfAbsent(connector.getName(), connector);
    }

    public interface Connector<T extends ConnectorParams> {
        String getName();

        ZscriptConnection makeConnection(ConnectorParams params);
    }

    public interface ConnectorParams {
    }
}
