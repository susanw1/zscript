package net.zscript.javaclient.connection;

import java.util.function.Consumer;

public interface ZscriptConnection extends AutoCloseable {
    // note, this should all be ByteBuffers...
    void send(byte[] data);

    void onReceive(Consumer<byte[]> handler);
}
