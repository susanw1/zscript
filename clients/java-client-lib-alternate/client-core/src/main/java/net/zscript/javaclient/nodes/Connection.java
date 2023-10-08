package net.zscript.javaclient.nodes;

import java.util.function.Consumer;

import net.zscript.javaclient.addressing.AddressedCommand;
import net.zscript.javaclient.addressing.AddressedResponse;

public interface Connection {
    void send(AddressedCommand cmd);

    void onReceive(Consumer<AddressedResponse> resp);
}
