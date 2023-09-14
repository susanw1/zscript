package net.zscript.javaclient.connection;

import java.io.Closeable;
import java.io.IOException;
import java.util.function.Consumer;

/**
 * Defines the underlying Connection mechanism required to talk Zscript messages to a device. We need to send bytes, and we need to handle bytes.
 */
public interface ZscriptConnection extends Closeable {
    /**
     * Sends the supplied bytes over this Connection. This should be one or more Zscript Command Sequences terminated with a '\n'. If this is a packet-based communication system,
     * like UDP, then the order of arrival of multiple sends is not guaranteed.
     *
     * @param data the bytes to send
     * @throws IOException if anything detectably goes wrong
     */
    // note, this should all be ByteBuffers...
    void send(byte[] data) throws IOException;

    /**
     * Sets the responseHandler for responses. Generally this must be set before the first send, and should reject being set more than once. The responseHandler must receive block
     * of bytes that represent complete Zscript response messages.
     *
     * @param responseHandler the responseHandler to process returned responses, never null
     */
    void onReceive(Consumer<byte[]> responseHandler);
}
