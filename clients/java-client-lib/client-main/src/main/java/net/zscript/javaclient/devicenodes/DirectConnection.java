package net.zscript.javaclient.devicenodes;

import java.io.Closeable;
import java.io.IOException;
import java.io.InputStream;
import java.io.UncheckedIOException;
import java.util.concurrent.CancellationException;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.concurrent.ScheduledExecutorService;
import java.util.function.BiConsumer;
import java.util.function.Consumer;

import static net.zscript.util.ByteString.byteString;

import org.slf4j.Logger;

import net.zscript.javaclient.addressing.AddressedCommand;
import net.zscript.javaclient.addressing.AddressedResponse;
import net.zscript.javaclient.addressing.CompleteAddressedResponse;
import net.zscript.javaclient.threading.ZscriptWorkerThread;
import net.zscript.javaclient.tokens.ExtendingTokenBuffer;
import net.zscript.model.components.Zchars;
import net.zscript.tokenizer.TokenBuffer;
import net.zscript.tokenizer.Tokenizer;
import net.zscript.util.ByteString;
import net.zscript.util.ByteString.ByteStringBuilder;

/**
 * Defines a connection which talks directly to a Zscript-enabled device, rather than via addressing through a parent node. It's the superclass of any hardware or network
 * interfacing Connections.
 * <p>
 * For receiving responses, there are two types of Direct Connection: push and pull. Push connections can call a call-back with their bytes when they arrive (as per
 * {@link #onReceiveBytes(Consumer)}). Pull connections have to call some kind of read method, and further divide into blocking vs non-blocking (poll on interval).
 * <p>
 * DirectConnections are responsible for ensuring that split-up sequences of bytes are bundled into newline-terminated response sequences.
 */
public abstract class DirectConnection implements Connection, Closeable {
    /** General thread pool for connection-related actions, eg blocking reads */
    private static final ScheduledExecutorService CONNECTION_EXEC = Executors.newScheduledThreadPool(0);

    private volatile Future<?> readingTaskFuture;

    private final ZscriptWorkerThread thread = new ZscriptWorkerThread();

    private final BiConsumer<byte[], Exception> parseFailHandler = (bytes, e) -> getLogger().warn("Response failed parsing: {}", byteString(bytes));
    private final ByteStringBuilder             receivedBytes    = ByteString.builder();

    protected DirectConnection() {
    }

    /**
     * Converts the supplied command to bytes and passes it to {@link #sendBytes(byte[])}
     *
     * @param cmd the command to be sent
     */
    @Override
    public final void send(AddressedCommand cmd) {
        try {
            final ByteString data = cmd.toByteString();
            getLogger().trace("Sending command: {}", data);
            sendBytes(data.toByteArray());
        } catch (IOException e) {
            throw new UncheckedIOException(e);
        }
    }

    /**
     * Registers the supplied handler to receive complete response sequences, by registering a byte[] callback on {@link #onReceiveBytes(Consumer)}. This method performs the
     * Zscript-parse of the response bytes into an {@link AddressedResponse} object before passing it to the supplied handler.
     *
     * @param responseHandler the response handler to register
     */
    @Override
    public final void onReceive(Consumer<AddressedResponse> responseHandler) {
        getLogger().atTrace().setMessage("register onReceive handler (using onReceiveBytes)").log();
        onReceiveBytes(bytes -> thread.moveOntoThread(() -> {
            getLogger().atTrace().setMessage("Received response: {}").addArgument(() -> byteString(bytes)).log();
            processReceivedBytes(bytes, responseHandler);
        }));
    }

    /**
     * Recombines broken consecutive sequences of bytes, parses them as AddressedResponses and passes them to the supplied handler (may result in zero or more calls, depending on
     * how many sequences are completed by the supplied bytes).
     *
     * @param bytes           the bytes that make up the response stream
     * @param responseHandler the handler to accept the parsed AddressedResponses
     */
    private void processReceivedBytes(byte[] bytes, Consumer<AddressedResponse> responseHandler) {
        int nlCount = ByteString.count(Zchars.Z_NEWLINE, bytes);
        if (nlCount == 0) {
            // these bytes are incomplete, so stash them
            receivedBytes.appendRaw(bytes);
        } else {
            final TokenBuffer buffer = new ExtendingTokenBuffer();
            final Tokenizer   t      = new Tokenizer(buffer.getTokenWriter(), true);

            // >0 incoming newlines, so we can use (and flush) any stashed bytes
            if (receivedBytes.size() > 0) {
                receivedBytes.build().forEach(t::accept);
                receivedBytes.reset();
            }

            // process the response sequences
            int index = 0;
            while (nlCount > 0 && index < bytes.length) {
                final byte b = bytes[index++];
                t.accept(b);
                if (b == Zchars.Z_NEWLINE) {
                    try {
                        AddressedResponse parsedResponse = CompleteAddressedResponse.parse(buffer.getTokenReader().getFirstReadToken()).asResponse();
                        responseHandler.accept(parsedResponse);
                    } catch (Exception e) {
                        parseFailHandler.accept(bytes, e);
                    }
                    nlCount--;
                }
            }

            // finally, stash any residual incomplete sequence for next time
            if (index < bytes.length) {
                receivedBytes.appendRaw(bytes, index, bytes.length - index);
            }
        }
    }

    @Override
    public void notifyResponseMatched(AddressedCommand foundCommand) {
        // direct connection does nothing here: this is just the notification from a child node that the foundCommand has been successfully matched to a response
    }

    /**
     * Should be implemented to provide the lower-level implementation of "sending Zscript command-sequence-encoded bytes" onto the Zscript Receiver devices. It is possible for
     * command sequences to be split across multiple consecutive calls, and implementations must handle that.
     *
     * @param data the data to send
     * @throws IOException if anything fails
     */
    public abstract void sendBytes(final byte[] data) throws IOException;

    /**
     * Should be defined to provide the lower-level implementation of "check for received Zscript response-sequence encoded bytes" (responses or notifications) from the Zscript
     * Receiver devices, and once read in, the response bytes should be passed to the supplied consumer.
     * <p>
     * Supports registering a single listener, once only.
     * <p>
     * This method is invoked by the {@link DirectConnection#onReceive(Consumer)} method, which parses the byte[] into {@link AddressedResponse}. The bytes may represent partial
     * responses, ie it is not required that the implementation send single complete sequences; the bytes may be split up over multiple successive in-order calls, and the
     * {@link #onReceive(Consumer)} method reassembles them.
     *
     * @param bytesResponseHandler the DirectConnection's consumer to process received bytes
     * @throws IllegalStateException if handler is already registered
     */
    public abstract void onReceiveBytes(final Consumer<byte[]> bytesResponseHandler);

    /**
     * Response-side helper method starts a background thread invoking {@link #blockingReadHelper(InputStream, Consumer)}, to read from the supplied stream and pass the bytes to
     * the responseHandler. Will reject calls if a handler is already registered.
     *
     * @param inResponseStream     the stream to read
     * @param bytesResponseHandler the handler to receive the response bytes
     */
    protected void startBlockingReadHelper(InputStream inResponseStream, final Consumer<byte[]> bytesResponseHandler) {
        if (readingTaskFuture != null) {
            throw new IllegalStateException("onReceiveBytes handler already registered");
        }
        readingTaskFuture = CONNECTION_EXEC.submit(() -> blockingReadHelper(inResponseStream, bytesResponseHandler));
        getLogger().trace("blockingReadHelper submitted");
    }

    /**
     * Response-side helper method that calls read() from a supplied stream and passes the bytes to the responseHandler
     *
     * @param inResponseStream     the stream to read
     * @param bytesResponseHandler the handler to receive the response bytes
     */
    protected void blockingReadHelper(final InputStream inResponseStream, final Consumer<byte[]> bytesResponseHandler) {
        try {
            final byte[] buf = new byte[1024];
            int          len;
            getLogger().trace("blockingReadHelper about to enter loop...");
            while (!Thread.interrupted() && (len = doRead(inResponseStream, buf)) != -1) {
                getLogger().trace("blockingReadHelper read something [len={}]", len);
                if (len > 0) {
                    final var byteStringBuilder = ByteString.builder().appendRaw(buf, 0, len);
                    getLogger().trace("blockingReadHelper passed on: {}", byteStringBuilder);
                    bytesResponseHandler.accept(byteStringBuilder.toByteArray());
                    getLogger().trace("blockingReadHelper accept() returned...");
                }
            }
            inResponseStream.close();
        } catch (IOException e) {
            throw new UncheckedIOException(e);
        } finally {
            getLogger().trace("blockingReadHelper terminated");
        }
    }

    // adds logging to highlight where we're blocking during response listening
    private int doRead(final InputStream inResponseStream, final byte[] buf) throws IOException {
        getLogger().trace("response doRead() entered");
        final int read = inResponseStream.read(buf);
        getLogger().trace("response doRead() return (return={})", read);
        return read;
    }

    @Override
    public void close() throws IOException {
        getLogger().trace("DirectConnection close()");
        final Future<?> r = readingTaskFuture;
        if (r != null) {
            r.cancel(true);
            try {
                r.get();
                getLogger().trace("DirectConnection response-reader completed and closed");
            } catch (CancellationException e) {
                getLogger().trace("DirectConnection response-reader cancelled");
            } catch (InterruptedException | ExecutionException e) {
                throw new IOException(e);
            } finally {
                readingTaskFuture = null;
            }
        }
    }

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
