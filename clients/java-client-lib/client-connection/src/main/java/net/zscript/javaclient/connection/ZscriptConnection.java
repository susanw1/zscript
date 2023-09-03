package net.zscript.javaclient.connection;

import java.io.Closeable;
import java.io.IOException;
import java.util.function.Consumer;

public interface ZscriptConnection extends Closeable {
    // note, this should all be ByteBuffers...
    void send(byte[] data) throws IOException;

    void onReceive(Consumer<byte[]> handler);
}
