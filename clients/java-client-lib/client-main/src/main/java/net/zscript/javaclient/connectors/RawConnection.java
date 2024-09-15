package net.zscript.javaclient.connectors;

import java.io.IOException;
import java.io.UncheckedIOException;
import java.nio.ByteBuffer;
import java.util.function.Consumer;

import static java.nio.charset.StandardCharsets.UTF_8;

import org.slf4j.Logger;

import net.zscript.javaclient.addressing.AddressedCommand;
import net.zscript.javaclient.addressing.AddressedResponse;
import net.zscript.javaclient.addressing.CompleteAddressedResponse;
import net.zscript.javaclient.nodes.Connection;
import net.zscript.javaclient.threading.ZscriptWorkerThread;
import net.zscript.javaclient.tokens.ExtendingTokenBuffer;
import net.zscript.tokenizer.Tokenizer;

public abstract class RawConnection implements Connection, AutoCloseable {
    private final ZscriptWorkerThread thread = new ZscriptWorkerThread();

    private final Consumer<byte[]> parseFailHandler = bytes -> getLogger().warn("Response failed parsing: {}", new String(bytes, UTF_8));

    protected abstract Logger getLogger();

    @Override
    public final void send(AddressedCommand cmd) {
        try {
            byte[] data = cmd.toByteString().toByteArray();
            if (getLogger().isTraceEnabled()) {
                getLogger().trace(UTF_8.decode(ByteBuffer.wrap(data)).toString());
            }
            send(data);
        } catch (IOException e) {
            throw new UncheckedIOException(e);
        }
    }

    @Override
    public final void onReceive(Consumer<AddressedResponse> resp) {
        onReceiveBytes(data -> thread.moveOntoThread(() -> {
            AddressedResponse parsed = null;
            try {
                ExtendingTokenBuffer buffer = new ExtendingTokenBuffer();
                Tokenizer            t      = new Tokenizer(buffer.getTokenWriter(), 2);
                for (byte b : data) {
                    t.accept(b);
                }
                if (getLogger().isTraceEnabled()) {
                    getLogger().trace("{}", new String(data, UTF_8));
                }
                parsed = CompleteAddressedResponse.parse(buffer.getTokenReader()).asResponse();
            } catch (Exception e) {
                parseFailHandler.accept(data);
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

    protected abstract void send(final byte[] data) throws IOException;

    protected abstract void onReceiveBytes(final Consumer<byte[]> responseHandler);

    @Override
    public ZscriptWorkerThread getAssociatedThread() {
        return thread;
    }
}
