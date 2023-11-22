package net.zscript.javaclient.connectors;

import static java.nio.charset.StandardCharsets.UTF_8;

import org.slf4j.Logger;

import java.io.IOException;
import java.io.UncheckedIOException;
import java.nio.ByteBuffer;
import java.util.function.Consumer;

import net.zscript.javaclient.addressing.AddressedCommand;
import net.zscript.javaclient.addressing.AddressedResponse;
import net.zscript.javaclient.addressing.CompleteAddressedResponse;
import net.zscript.javaclient.nodes.Connection;
import net.zscript.javaclient.threading.ZscriptWorkerThread;
import net.zscript.javareceiver.tokenizer.TokenExtendingBuffer;
import net.zscript.javareceiver.tokenizer.Tokenizer;

public abstract class RawConnection implements Connection, AutoCloseable {
    private final ZscriptWorkerThread thread = new ZscriptWorkerThread();

    protected abstract Logger getLogger();

    @Override
    public final void send(AddressedCommand cmd) {
        try {
            byte[] data = cmd.toBytes().toByteArray();
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
            TokenExtendingBuffer buffer = new TokenExtendingBuffer();
            Tokenizer            t      = new Tokenizer(buffer.getTokenWriter(), 2);
            for (byte b : data) {
                t.accept(b);
            }
            if (getLogger().isTraceEnabled()) {
                getLogger().trace("{}", new String(data, UTF_8));
            }
            resp.accept(CompleteAddressedResponse.parse(buffer.getTokenReader()).asResponse());
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
