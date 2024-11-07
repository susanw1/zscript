package net.zscript.javareceiver.testing;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.UncheckedIOException;
import java.util.Iterator;
import java.util.concurrent.Executor;
import java.util.concurrent.Executors;
import java.util.concurrent.atomic.AtomicReference;
import java.util.function.Consumer;

import net.zscript.javareceiver.core.ZscriptChannel;
import net.zscript.javareceiver.execution.CommandContext;
import net.zscript.tokenizer.TokenBuffer;
import net.zscript.tokenizer.Tokenizer;
import net.zscript.util.ByteString;
import net.zscript.util.ByteString.ByteStringBuilder;

/**
 * Channel designed for testing. It reads commands from an InputStream and sends responses/notifications to an OutputStream.
 * <p>
 * It runs a (I/O-blocking) reader thread for copying the commandStream into a TokenBuffer, byte by byte.
 */
public class LocalChannel extends ZscriptChannel {
    private static final Executor channelReadingThreads = Executors.newCachedThreadPool();

    private final InputStream commandStream;
    private final Tokenizer   tokenizer;

    /** Accumulation buffer for bytes from background thread */
    private final AtomicReference<ByteStringBuilder> byteAccumulator;
    private       Iterator<Byte>                     byteIterator; // current bytes, or null if we're waiting for more

    /**
     * Channel reads commands and sends responses/notifications.
     *
     * @param commandStream   the source of commands
     * @param responseHandler the consumer to send completed response sequences
     */
    public LocalChannel(TokenBuffer buffer, InputStream commandStream, Consumer<byte[]> responseHandler) {
        super(buffer, new OutputStreamOutStream<>(new ByteArrayOutputStream()) {
            @Override
            public void close() {
                responseHandler.accept(getOutputStream().toByteArray());
                getOutputStream().reset();
            }
        });
        this.commandStream = commandStream;
        this.tokenizer = new Tokenizer(buffer.getTokenWriter());

        this.byteAccumulator = new AtomicReference<>(ByteString.builder());
        this.byteIterator = null;

        channelReadingThreads.execute(this::reader);
    }

    /**
     * Feeds bytes from the current byteIterator into the tokenizer. If there is no byteIterator, but there are accumulated bytes from the reader thread, grab the accumulator's
     * bytes' iterator. The byteIterator is null if we're awaiting more bytes.
     */
    @Override
    public void moveAlong() {
        // if we've not got any bytes to iterate, but there are more accumulated, then take them.
        if (byteIterator == null && byteAccumulator.get().size() > 0) {
            ByteStringBuilder existingBytes = byteAccumulator.getAndUpdate(b -> ByteString.builder());
            byteIterator = existingBytes.build().iterator();
        }
        // if there are bytes to iterate, then process them
        if (byteIterator != null) {
            while (byteIterator.hasNext() && tokenizer.checkCapacity()) {
                tokenizer.accept(byteIterator.next());
            }
            // if we've processed all the bytes in this chunk, then mark byteIterator as exhausted by setting it to null.
            if (!byteIterator.hasNext()) {
                byteIterator = null;
            }
        }
    }

    /**
     * Blocking read copies each byte to the queue.
     */
    private void reader() {
        try (InputStream s = commandStream) {
            int b;
            while ((b = s.read()) != -1) {
                byteAccumulator.get().appendByte(b);
            }
        } catch (IOException ex) {
            throw new UncheckedIOException(ex);
        }
    }

    @Override
    public void channelInfo(CommandContext ctx) {
        ctx.getOutStream().writeBigFieldQuoted("LocalChannel");
    }

    @Override
    public void channelSetup(CommandContext ctx) {
        // nothing to do here
    }
}
