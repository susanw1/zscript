package net.zscript.javaclient.responseparser;

import java.util.function.Consumer;

public interface ZscriptConnection {
    void send(byte[] data);

    void onReceive(Consumer<byte[]> handler);
}
