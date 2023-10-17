package net.zscript.javaclient.connectors;

import java.io.IOException;
import java.io.UncheckedIOException;
import java.util.function.Consumer;

import net.zscript.javaclient.addressing.AddressedCommand;
import net.zscript.javaclient.addressing.AddressedResponse;
import net.zscript.javaclient.addressing.CompleteAddressedResponse;
import net.zscript.javaclient.nodes.Connection;
import net.zscript.javareceiver.tokenizer.TokenExtendingBuffer;
import net.zscript.javareceiver.tokenizer.Tokenizer;

public abstract class RawConnection implements Connection, AutoCloseable {

    @Override
    public final void send(AddressedCommand cmd) {
        try {
            send(cmd.toBytes().toByteArray());
        } catch (IOException e) {
            throw new UncheckedIOException(e);
        }
    }

    @Override
    public final void onReceive(Consumer<AddressedResponse> resp) {
        onReceiveBytes(data -> {
            TokenExtendingBuffer buffer = new TokenExtendingBuffer();
            Tokenizer            t      = new Tokenizer(buffer.getTokenWriter(), 2);
            for (byte b : data) {
                t.accept(b);
            }
            resp.accept(CompleteAddressedResponse.parse(buffer.getTokenReader()).asResponse());
        });
    }

    @Override
    public void responseReceived(AddressedCommand found) {
        // do nothing
    }

    protected abstract void send(final byte[] data) throws IOException;

    protected abstract void onReceiveBytes(final Consumer<byte[]> responseHandler);

}
