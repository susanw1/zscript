package net.zscript.javareceiver.testing;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InterruptedIOException;
import java.io.OutputStream;
import java.io.PipedInputStream;
import java.io.PipedOutputStream;
import java.util.Iterator;
import java.util.Queue;
import java.util.concurrent.CancellationException;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.function.Consumer;

import static net.zscript.util.ByteString.byteString;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import net.zscript.javareceiver.core.ZscriptChannel;
import net.zscript.javareceiver.execution.CommandContext;
import net.zscript.tokenizer.TokenBuffer;
import net.zscript.tokenizer.Tokenizer;
import net.zscript.util.ByteString;

/**
 * Channel designed for testing. It reads commands from an InputStream and sends responses/notifications to an OutputStream.
 * <p>
 * It runs a (I/O-blocking) reader thread for copying the commandStream into a TokenBuffer, byte by byte.
 */
public class LocalChannel extends ZscriptChannel {
    private static final Logger LOG = LoggerFactory.getLogger(LocalChannel.class);

    private static final ExecutorService channelReadingThreads = Executors.newCachedThreadPool();

    private       Future<Object> commandReaderFuture;
    private final Tokenizer      tokenizer;

    // ZscriptChannel will read commands from here (async), connected to Connection's OutputStream.
    private final PipedInputStream channelCommandInput;

    // We expose this to client-side connection for writing commands, which are read out into the tokenBuffer
    private final PipedOutputStream connCommandOutput;
    // Client-side connection will read from this to get responses
    private final PipedInputStream  connResponseInput;

    /** Accumulation buffer for bytes from background thread */
    private final Queue<ByteString> incoming;
    private       Iterator<Byte>    byteIterator; // current bytes, or null if we're waiting for more

    private ChannelStatus status;

    enum ChannelStatus {
        OK,
        CLOSED,
        INTERRUPTED,
        ERROR
    }

    /**
     * Channel that reads commands (see {@link #getCommandOutputStream()}) and sends responses/notifications (see {@link #getResponseInputStream()}).
     *
     * @param buffer the buffer for writing incoming commands
     */
    public LocalChannel(TokenBuffer buffer) {
        this(buffer, new OutputStreamOutStream<>(new PipedOutputStream()), true);
    }

    /**
     * Channel reads commands and sends responses/notifications.
     *
     * @param buffer          the buffer for writing incoming commands
     * @param responseHandler the consumer to send completed response sequences
     */
    public LocalChannel(TokenBuffer buffer, Consumer<byte[]> responseHandler) {
        this(buffer, new OutputStreamOutStream<>(new ByteArrayOutputStream()) {
            @Override
            public void close() {
                responseHandler.accept(getOutputStream().toByteArray());
                getOutputStream().reset();
            }
        }, false);
    }

    private LocalChannel(TokenBuffer buffer, OutputStreamOutStream<?> out, boolean createPipe) {
        super(buffer, out);
        this.tokenizer = new Tokenizer(buffer.getTokenWriter());

        this.incoming = new ConcurrentLinkedQueue<>();
        this.byteIterator = null;

        try {
            connCommandOutput = new PipedOutputStream();
            channelCommandInput = new PipedInputStream(connCommandOutput);

            connResponseInput = createPipe ? new PipedInputStream((PipedOutputStream) out.getOutputStream()) : null;
        } catch (IOException e) {
            throw new InternalError("Error creating piped stream - this shouldn't happen!", e);
        }

        start();
    }

    /**
     * An OutputStream for writing commands to this Channel. Ensure that {@link OutputStream#flush()} is called when the command data is sufficiently complete for processing.
     *
     * @return an OutputStream for writing commands
     */
    public OutputStream getCommandOutputStream() {
        return connCommandOutput;
    }

    /**
     * An InputStream for reading responses from this Channel.
     *
     * @return an InputStream
     * @throws IllegalStateException if this {@link LocalChannel} was created with a callback
     */
    public InputStream getResponseInputStream() {
        if (connResponseInput == null) {
            throw new IllegalStateException("this LocalChannel provides a response callback, not a stream");
        }
        return connResponseInput;
    }

    private void start() {
        this.status = ChannelStatus.OK;
        this.commandReaderFuture = channelReadingThreads.submit(() -> {
            commandReader();
            return null;
        });
    }

    /**
     * Feeds bytes from the current byteIterator into the tokenizer. If there is no byteIterator, but there are accumulated bytes from the reader thread, grab the accumulator's
     * bytes' iterator. The byteIterator is null if we're awaiting more bytes.
     */
    @Override
    public void moveAlong() {
        // if we've not got any bytes to iterate, but there are more accumulated, then take them.
        if (byteIterator == null && !incoming.isEmpty()) {
            final ByteString incomingBytes = incoming.poll();
            LOG.debug("Channel.moveAlong() processing command bytes: {}", incomingBytes);
            byteIterator = incomingBytes.iterator();
        }
        // if there are bytes to iterate, then process them
        if (byteIterator != null) {
            int byteCount;
            for (byteCount = 0; byteIterator.hasNext() && tokenizer.checkCapacity(); byteCount++) {
                final byte b = byteIterator.next();
                LOG.trace("moveAlong() tokenizing: {} ('{}')", b, (char) b);
                tokenizer.accept(b);
            }
            // if we've processed all the bytes in this chunk, then mark byteIterator as exhausted by setting it to null.
            if (!byteIterator.hasNext()) {
                byteIterator = null;
            }
            LOG.trace("moveAlong() moved {} command bytes", byteCount);
        }
    }

    /**
     * Blocking read copies each incoming byte to the queue.
     */
    private void commandReader() {
        try (InputStream s = channelCommandInput) {
            readBytesToQueue(s);
        } catch (InterruptedIOException ex) {
            LOG.error("commandReader() interrupted");
            this.status = ChannelStatus.INTERRUPTED;
        } catch (Exception ex) {
            LOG.error("commandReader() died", ex);
            this.status = ChannelStatus.ERROR;
        }
    }

    // VisibleForTesting
    void readBytesToQueue(InputStream s) throws IOException {
        byte[] buf = new byte[128];
        int    len;
        while ((len = s.read(buf)) != -1) {
            if (len > 0) {
                incoming.add(byteString(buf, 0, len));
            }
        }
        this.status = ChannelStatus.CLOSED;
    }

    @Override
    public void channelInfo(CommandContext ctx) {
        ctx.getOutStream().writeBigFieldQuoted("LocalChannel");
        //        ctx.getOutStream().writeField('B', tokenizer.getBufferLength());  // bufferLength
    }

    @Override
    public void channelSetup(CommandContext ctx) {
        if (ctx.hasField('R')) {
            if (commandReaderFuture == null || commandReaderFuture.isDone()) {
                start();
            } else {
                commandReaderFuture.cancel(true);
                try {
                    commandReaderFuture.get();
                    status = ChannelStatus.OK;
                } catch (CancellationException e) {
                    LOG.warn("channelSetup() cancelled", e);
                } catch (InterruptedException e) {
                    this.status = ChannelStatus.INTERRUPTED;
                    LOG.warn("channelSetup() interrupted", e);
                } catch (ExecutionException e) {
                    this.status = ChannelStatus.ERROR;
                    LOG.error("channelSetup() died", e);
                }
                commandReaderFuture = null;
            }
        }
    }

    ChannelStatus getStatus() {
        return status;
    }
}
