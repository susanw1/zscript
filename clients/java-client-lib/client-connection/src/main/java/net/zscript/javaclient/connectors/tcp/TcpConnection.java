package net.zscript.javaclient.connectors.tcp;

import java.net.Socket;
import java.util.function.Consumer;

import net.zscript.javaclient.connection.ZscriptConnection;

public class TcpConnection implements ZscriptConnection {
    private final Socket socket;

    public TcpConnection(TcpConnectionParams params) {
        socket = new Socket();
    }

    @Override
    public void send(byte[] data) {

    }

    @Override
    public void onReceive(Consumer<byte[]> handler) {

    }

    @Override
    public void close() throws Exception {
    }

    private static class TcpConnectionParams {
        private String address;
        private int    port;

    }
}
