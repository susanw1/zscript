package net.zscript.javaclient.nodes;

import java.util.function.Consumer;

import net.zscript.javaclient.addressing.AddressedCommand;
import net.zscript.javaclient.addressing.AddressedResponse;
import net.zscript.javaclient.threading.ZscriptWorkerThread;

public interface Connection {
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

    /**
     * Notification that the supplied command has been matched to a response.
     *
     * @param foundCommand the command that has been found
     */
    void notifyResponseMatched(AddressedCommand foundCommand);

    ZscriptWorkerThread getAssociatedThread();
}
