package net.zscript.javaclient.devicenodes;

import java.util.function.Consumer;

public interface Connection extends ResponseMatchListener {
    /**
     * Sends the supplied command up towards to root node of the device tree, or transforms it to bytes for transmission over the wire.
     *
     * @param cmd the command to send
     */
    void send(AddressedCommand cmd);

    /**
     * Registers a handler for incoming responses to be passed down to child nodes, or (on arrival at the sending node) for matching with a corresponding command.
     *
     * @param responseHandler the response handler to register
     */
    void onReceive(Consumer<AddressedResponse> responseHandler);

    ZscriptWorkerThread getAssociatedThread();
}
