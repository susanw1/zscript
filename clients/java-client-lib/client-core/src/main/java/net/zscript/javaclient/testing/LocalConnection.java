package net.zscript.javaclient.testing;

import javax.annotation.Nullable;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.function.Consumer;

import static java.util.Objects.requireNonNull;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import net.zscript.javaclient.nodes.DirectConnection;

/**
 * Connector designed for testing. It allows commands to be written to an OutputStream, and responses/notifications to be read from an InputStream.
 */
public class LocalConnection extends DirectConnection {
    private static final Logger LOG = LoggerFactory.getLogger(LocalConnection.class);

    private OutputStream commandStream;
    private InputStream  responseStream;

    @Nullable
    private Consumer<byte[]> responseHandler;

    /**
     * Basic Connection that allows commands to be written to an outputStream, and responses/notifications to be read from an InputStream.
     *
     * @param commandStream  commands are written here
     * @param responseStream responses are read from here
     */
    public LocalConnection(OutputStream commandStream, InputStream responseStream) {
        this.commandStream = requireNonNull(commandStream, "commandStream");
        this.responseStream = requireNonNull(responseStream, "responseStream");
        this.responseHandler = null;
    }

    @Override
    protected void send(byte[] data) throws IOException {
        commandStream.write(data);
    }

    /**
     * @param responseHandler a consumer to process received bytes; if null, then receiver is unset
     */
    @Override
    protected void onReceiveBytes(@Nullable Consumer<byte[]> responseHandler) {
        this.responseHandler = responseHandler;
    }

    @Override
    public void close() throws Exception {
        commandStream.close();
    }

    @Override
    protected Logger getLogger() {
        return LOG;
    }
}
