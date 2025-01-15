package net.zscript.javaclient.testing;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.function.Consumer;

import static java.util.Objects.requireNonNull;
import static net.zscript.util.ByteString.byteString;
import static net.zscript.util.ByteString.lastIndexOf;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import net.zscript.javaclient.devicenodes.DirectConnection;
import net.zscript.model.components.Zchars;

/**
 * Connector designed for testing. It allows commands to be written to an OutputStream, and responses/notifications to be read from an InputStream.
 */
public class LocalConnection extends DirectConnection {
    private static final Logger LOG = LoggerFactory.getLogger(LocalConnection.class);

    private final OutputStream commandStream;
    private final InputStream  responseStream;

    /**
     * Basic Connection that allows commands to be written to an outputStream, and responses/notifications to be read from an InputStream.
     *
     * @param commandStream  commands are written here
     * @param responseStream responses are read from here
     */
    public LocalConnection(OutputStream commandStream, InputStream responseStream) {
        this.commandStream = requireNonNull(commandStream, "commandStream");
        this.responseStream = requireNonNull(responseStream, "responseStream");
    }

    @Override
    public void sendBytes(byte[] data) throws IOException {
        LOG.atTrace().addArgument(() -> byteString(data)).log("sendBytes: [{}]");
        commandStream.write(data);

        // check from end as it's likely data is a complete '\n'-terminated line of Zscript
        if (lastIndexOf(Zchars.Z_NEWLINE, data.length, data) != -1) {
            commandStream.flush();
        }
    }

    /**
     * @param bytesResponseHandler the DirectConnection's consumer to process received bytes
     */
    @Override
    public void onReceiveBytes(Consumer<byte[]> bytesResponseHandler) {
        startBlockingReadHelper(responseStream, bytesResponseHandler);
    }

    @Override
    public void close() throws IOException {
        commandStream.close();
        super.close();
    }

    @Override
    protected Logger getLogger() {
        return LOG;
    }
}
