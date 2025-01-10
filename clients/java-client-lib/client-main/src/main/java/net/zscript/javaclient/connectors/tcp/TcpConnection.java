package net.zscript.javaclient.connectors.tcp;

import java.io.BufferedInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.util.function.Consumer;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import net.zscript.javaclient.nodes.DirectConnection;

public class TcpConnection extends DirectConnection {
    private static final Logger LOG = LoggerFactory.getLogger(TcpConnection.class);
    private final        Socket socket;

    private final InputStream  sockIn;
    private final OutputStream sockOut;

    @Override
    protected Logger getLogger() {
        return LOG;
    }

    public TcpConnection(Socket socket) throws IOException {
        this.socket = socket;
        this.sockOut = socket.getOutputStream();
        this.sockIn = new BufferedInputStream(socket.getInputStream());
    }

    public void sendBytes(final byte[] data) throws IOException {
        sockOut.write(data);
    }

    @Override
    public void onReceiveBytes(final Consumer<byte[]> bytesResponseHandler) {
        startBlockingReadHelper(sockIn, bytesResponseHandler);
    }

    //    private void blockingReadAndHandle() {
    //        try {
    //            ByteString.ByteStringBuilder builder = ByteString.builder();
    //            while (!Thread.interrupted()) {
    //                int ch;
    //                while ((ch = in.read()) != -1) {
    //                    builder.appendByte(ch);
    //                    if (ch == '\n') {
    //                        bytesResponseHandler.accept(builder.toByteArray());
    //                        builder = ByteString.builder();
    //                        break;
    //                    }
    //                }
    //            }
    //            in.close();
    //            socket.close();
    //        } catch (IOException e) {
    //            throw new UncheckedIOException(e);
    //        } finally {
    //            try {
    //                socket.close();
    //            } catch (IOException e) {
    //                throw new UncheckedIOException(e);
    //            }
    //        }
    //    }

    @Override
    public void close() throws IOException {
        sockOut.close();
        super.close();
    }

    @Override
    public String toString() {
        return socket.toString();
    }
}
