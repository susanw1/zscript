package net.zscript.javaclient.devicenodes;

import java.lang.reflect.Proxy;
import java.util.concurrent.TimeUnit;
import java.util.function.Consumer;

import net.zscript.javaclient.commandpaths.CommandExecutionPath;
import net.zscript.javaclient.commandpaths.ResponseExecutionPath;
import net.zscript.javaclient.commandpaths.ZscriptAddress;
import net.zscript.javaclient.sequence.CommandSequence;
import net.zscript.javaclient.sequence.ResponseSequence;

/**
 * Defines the core interface for a device as a node in the zscript device-tree.
 */
public interface ZscriptNode {
    static ZscriptNode newNode(Connection parentConnection) {
        return newNode(parentConnection, 128, 100, TimeUnit.MILLISECONDS);
    }

    static ZscriptNode newNode(Connection parentConnection, int bufferSize, long minSegmentChangeTime, TimeUnit unit) {
        ZscriptWorkerThread thread = parentConnection.getAssociatedThread();
        ZscriptBasicNode    node   = new ZscriptBasicNode(thread.getCallbackPool(), parentConnection, bufferSize, minSegmentChangeTime, unit);
        thread.addTimeoutCheck(node::checkTimeouts);

        return (ZscriptNode) Proxy.newProxyInstance(ZscriptNode.class.getClassLoader(), new Class[] { ZscriptNode.class },
                (obj, method, params) -> thread.moveOntoThread(() -> method.invoke(node, params)));
    }

    void setUnknownResponseHandler(Consumer<AddressedResponse> unknownResponseHandler);

    void setStrategy(QueuingStrategy strategy);

    void setBufferSize(int bufferSize);

    void setCallbackExceptionHandler(Consumer<Exception> callbackExceptionHandler);

    /**
     * Creates a Connection associated with the supplied Address.
     *
     * @param address an address to attach - must not already be attached
     * @return a Connection that can be used as the parentConnection to a sub-node
     */
    Connection attach(ZscriptAddress address);

    /**
     * Detaches the Connection associated with the supplied Address.
     *
     * @param address the address to detach
     * @return the previously attached Connection, or null if none
     */
    Connection detach(ZscriptAddress address);

    /**
     * Determines whether the supplied address corresponds to a known Connection
     *
     * @param address the address to check
     * @return true if attached, false otherwise
     */
    boolean isAttached(ZscriptAddress address);

    /**
     * Sends an unaddressed command sequence (optionally with locks and echo field), and posts the matching response sequence to the supplied callback.
     *
     * @param seq      the sequence to send
     * @param callback the handler for the response sequence
     */
    void send(CommandSequence seq, Consumer<ResponseSequence> callback);

    /**
     * Sends the supplied command path (without address, locks or echo), and posts the matching response to the supplied callback.
     *
     * @param path     the command sequence to send
     * @param callback the handler for the response sequence
     */
    void send(CommandExecutionPath path, Consumer<ResponseExecutionPath> callback);

    void forward(AddressedCommand addr);

    void setNotificationHandler(int notification, Consumer<ResponseSequence> handler);

    void removeNotificationHandler(int notification);
}
