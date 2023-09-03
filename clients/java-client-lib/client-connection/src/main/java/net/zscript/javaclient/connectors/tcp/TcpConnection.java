package net.zscript.javaclient.connectors.tcp;

import java.io.BufferedInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.UncheckedIOException;
import java.net.Socket;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.function.Consumer;

import net.zscript.javaclient.connection.ZscriptConnection;
import net.zscript.javaclient.connectors.ZscriptConnectors.ConnectorParams;

public class TcpConnection implements ZscriptConnection {
    private final Socket       socket;
    private final InputStream  in;
    private final OutputStream out;

    private Future<?>        future;
    private Consumer<byte[]> handler;

    public TcpConnection(TcpConnectionParams params) throws IOException {
        this.socket = new Socket(params.address, params.port);
        this.out = socket.getOutputStream();
        this.in = new BufferedInputStream(socket.getInputStream());
    }

    public void send(final byte[] data) throws IOException {
        out.write(data);
    }

    @Override
    public void onReceive(final Consumer<byte[]> handler) {
        if (this.handler != null || handler == null) {
            throw new IllegalStateException("Handler already assigned");
        }

        this.handler = handler;
        this.future = Executors.newSingleThreadExecutor().submit(this::readAndHandle);
    }

    private void readAndHandle() {
        try {
            ByteArrayOutputStream baos = new ByteArrayOutputStream();
            while (!Thread.interrupted()) {
                int ch;
                while ((ch = in.read()) != -1) {
                    baos.write(ch);
                    if (ch == '\n') {
                        handler.accept(baos.toByteArray());
                        baos.reset();
                        break;
                    }
                }
            }
            in.close();
            socket.close();
        } catch (IOException e) {
            throw new UncheckedIOException(e);
        } finally {
            try {
                socket.close();
            } catch (IOException e) {
                throw new UncheckedIOException(e);
            }
        }
    }

    @Override
    public void close() throws IOException {
        out.close();
        future.cancel(true);
        try {
            future.get();
        } catch (InterruptedException | ExecutionException e) {
            throw new RuntimeException(e);
        }
    }

    @Override
    public String toString() {
        return socket.toString();
    }

    public static class TcpConnectionParams implements ConnectorParams {
        private String address;
        private int    port;

        public TcpConnectionParams(String address, int port) {
            this.address = address;
            this.port = port;
        }
    }
}
