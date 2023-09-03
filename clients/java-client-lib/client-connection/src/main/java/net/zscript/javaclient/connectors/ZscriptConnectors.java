package net.zscript.javaclient.connectors;

import java.io.IOException;
import java.util.Collection;
import java.util.Map;
import java.util.Optional;
import java.util.concurrent.ConcurrentHashMap;

import net.zscript.javaclient.connection.ZscriptConnection;

public class ZscriptConnectors {
    private static Map<Class<? extends ConnectorParams>, Connector<?>> registeredConnectors = new ConcurrentHashMap<>();
    private static Map<ConnectorType, Class<Connector>>                connectorClassByType = new ConcurrentHashMap<>();

    public static Collection<Connector<?>> getAvailableConnectors() {
        return registeredConnectors.values();
    }

    public static <T extends ConnectorParams> Optional<Connector<T>> getConnector(Class<T> paramType) {
        @SuppressWarnings("unchecked")
        Connector<T> value = (Connector<T>) registeredConnectors.get(paramType);
        return Optional.ofNullable(value);
    }

    public static <T extends ConnectorParams> void register(Connector<T> connector) {
        registeredConnectors.putIfAbsent(connector.getConnectorParamsClass(), connector);
    }

    public interface Connector<T extends ConnectorParams> {
        String getType();

        Class<T> getConnectorParamsClass();

        ZscriptConnection makeConnection(T params) throws IOException;
    }

    public interface ConnectorParams {
    }

    public interface ConnectorType {
    }
}
