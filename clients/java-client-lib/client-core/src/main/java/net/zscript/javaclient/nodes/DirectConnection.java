package net.zscript.javaclient.nodes;

import java.io.IOException;
import java.io.UncheckedIOException;
import java.util.function.Consumer;

import static java.nio.charset.StandardCharsets.UTF_8;

import org.slf4j.Logger;

import net.zscript.javaclient.addressing.AddressedCommand;
import net.zscript.javaclient.addressing.AddressedResponse;
import net.zscript.javaclient.addressing.CompleteAddressedResponse;
import net.zscript.javaclient.threading.ZscriptWorkerThread;
import net.zscript.javaclient.tokens.ExtendingTokenBuffer;
import net.zscript.tokenizer.Tokenizer;
import net.zscript.util.ByteString;

/**
 * Defines a connection which talks directly to a Zscript-enabled device, rather than via addressing to a parent node. It's the superclass of any hardware or network interfacing
 * Connections.
 */
public abstract class DirectConnection implements Connection, AutoCloseable {
    private final ZscriptWorkerThread thread = new ZscriptWorkerThread();

    private final Consumer<byte[]> parseFailHandler = bytes -> getLogger().warn("Response failed parsing: {}", new String(bytes, UTF_8));

    @Override
    public final void send(AddressedCommand cmd) {
        try {
            final ByteString data = cmd.toByteString();
            if (getLogger().isTraceEnabled()) {
                getLogger().trace(data.toString());
            }
            send(data.toByteArray());
        } catch (IOException e) {
            throw new UncheckedIOException(e);
        }
    }

    @Override
    public final void onReceive(Consumer<AddressedResponse> resp) {
        onReceiveBytes(bytes -> thread.moveOntoThread(() -> {
            AddressedResponse parsed = null;
            try {
                ExtendingTokenBuffer buffer = new ExtendingTokenBuffer();
                Tokenizer            t      = new Tokenizer(buffer.getTokenWriter(), 2);
                for (byte b : bytes) {
                    t.accept(b);
                }
                if (getLogger().isTraceEnabled()) {
                    getLogger().trace("Received: {}", ByteString.from(bytes));
                }
                parsed = CompleteAddressedResponse.parse(buffer.getTokenReader()).asResponse();
            } catch (Exception e) {
                parseFailHandler.accept(bytes);
            }
            if (parsed != null) {
                // don't want to catch exceptions from here:
                resp.accept(parsed);
            }
        }));
    }

    @Override
    public void responseReceived(AddressedCommand found) {
        // do nothing
    }

    /**
     * Provides the lower-level implementation of "sending Zscript command-sequence-encoded bytes" onto the Zscript Receiver devices.
     *
     * @param data the data to send
     * @throws IOException if anything fails
     */
    protected abstract void send(final byte[] data) throws IOException;

    /**
     * Provides the lower-level implementation of "received Zscript response-sequence-encoded bytes" (responses or notifications) from the Zscript Receiver devices.
     * <p>
     * Supports registering a single listener, where setting another replaces the previous, and null unsets any receiver.
     * <p>
     * This method is invoked by the {@link #onReceive(Consumer)} method, which parses the byte[] into {@link AddressedResponse}.
     * <p>
     * TODO: determine whether these byte[] must be "complete", ie '\n' terminated? If they're fragmented, whose job is it to reassemble them?
     *
     * @param responseHandler a consumer to process received bytes, or null to unset
     */
    protected abstract void onReceiveBytes(final Consumer<byte[]> responseHandler);

    @Override
    public ZscriptWorkerThread getAssociatedThread() {
        return thread;
    }

    /**
     * Accesses the specific Logger for this Connection subclass.
     *
     * @return a logger
     */
    protected abstract Logger getLogger();
}
