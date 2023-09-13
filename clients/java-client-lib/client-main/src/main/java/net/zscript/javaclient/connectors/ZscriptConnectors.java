package net.zscript.javaclient.connectors;

import java.io.IOException;
import java.util.Collection;
import java.util.Map;
import java.util.Optional;
import java.util.concurrent.ConcurrentHashMap;
import java.util.function.Consumer;

import net.zscript.javaclient.connection.ZscriptConnection;
import net.zscript.javaclient.connectors.serial.SerialConnector;
import net.zscript.javaclient.connectors.tcp.TcpConnector;

public class ZscriptConnectors {
    public static final TcpConnector    TCP    = new TcpConnector();
    public static final SerialConnector SERIAL = new SerialConnector();

    public interface ZscriptConnector<T> {
        ZscriptConnection connect(T underlyingImpl) throws IOException;

        /**
         * Starts a scan for possible connection, and posts each possible hit to the supplied Consumer.
         *
         * @param scanResult
         */
        void scan(Consumer<T> scanResult);
    }
}
