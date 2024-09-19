package net.zscript.javaclient.connectors.tcp;

import javax.annotation.Nonnull;
import java.io.IOException;
import java.net.Socket;
import java.util.function.Consumer;

import net.zscript.javaclient.connectors.ZscriptConnectors.ZscriptConnector;

public class TcpConnector implements ZscriptConnector<Socket> {
    public TcpConnector() {
    }

    @Nonnull
    @Override
    public TcpConnection connect(Socket socket) throws IOException {
        return new TcpConnection(socket);
    }

    @Override
    public void scan(Consumer<Socket> scanResult) {
        // TCP can't scan!
    }
}
