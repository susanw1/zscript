package net.zscript.javaclient.nodes;

import java.util.function.Consumer;

import net.zscript.javaclient.addressing.AddressedCommand;
import net.zscript.javaclient.addressing.AddressedResponse;
import net.zscript.javaclient.threading.ZscriptCallbackThreadpool;
import net.zscript.javaclient.threading.ZscriptWorkerThread;

public interface Connection {
    void send(AddressedCommand cmd);

    void onReceive(Consumer<AddressedResponse> resp);

    void responseReceived(AddressedCommand found);

    ZscriptWorkerThread getAssociatedThread();
}
