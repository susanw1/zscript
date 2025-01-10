package net.zscript.javaclient.connectors;

import javax.annotation.Nonnull;
import java.io.IOException;
import java.util.function.Consumer;

import net.zscript.javaclient.connectors.serial.SerialConnector;
import net.zscript.javaclient.connectors.tcp.TcpConnector;
import net.zscript.javaclient.nodes.DirectConnection;

public class ZscriptConnectors {
    public static final TcpConnector    TCP    = new TcpConnector();
    public static final SerialConnector SERIAL = new SerialConnector();

    public interface ZscriptConnector<T> {
        @Nonnull
        DirectConnection connect(T underlyingImpl) throws IOException;

        /**
         * Starts a scan for possible connection, and posts each possible hit to the supplied Consumer.
         *
         * @param scanResultListener recipient for result notifications
         */
        void scan(Consumer<T> scanResultListener);
    }
}
