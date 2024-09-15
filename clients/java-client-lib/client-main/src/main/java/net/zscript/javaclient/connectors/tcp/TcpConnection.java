package net.zscript.javaclient.connectors.tcp;

import java.io.BufferedInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.UncheckedIOException;
import java.net.Socket;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.function.Consumer;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import net.zscript.javaclient.connectors.RawConnection;

public class TcpConnection extends RawConnection {
    private static final Logger          LOG = LoggerFactory.getLogger(TcpConnection.class);
    private final        Socket          socket;
    private final        ExecutorService executor;

    private final InputStream  in;
    private final OutputStream out;

    private Future<?>        future;
    private Consumer<byte[]> handler;

    @Override
    protected Logger getLogger() {
        return LOG;
    }

    public TcpConnection(Socket socket) throws IOException {
        this(socket, Executors.newSingleThreadExecutor());
    }

    public TcpConnection(Socket socket, ExecutorService executor) throws IOException {
        this.socket = socket;
        this.executor = executor;

        this.out = socket.getOutputStream();
        this.in = new BufferedInputStream(socket.getInputStream());
    }

    public void send(final byte[] data) throws IOException {
        out.write(data);
    }

    @Override
    public void onReceiveBytes(final Consumer<byte[]> responseHandler) {
        if (this.handler != null) {
            throw new IllegalStateException("Handler already assigned");
        }

        this.handler = responseHandler;
        this.future = executor.submit(this::readAndHandle);
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
}
