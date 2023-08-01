package net.zscript.javaclient.responseParser;

import java.util.function.Consumer;

public interface ZscriptConnection {
    void send(byte[] data);

    void onReceive(Consumer<byte[]> handler);
}
