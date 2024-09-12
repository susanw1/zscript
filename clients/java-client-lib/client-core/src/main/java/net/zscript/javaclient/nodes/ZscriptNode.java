package net.zscript.javaclient.nodes;

import java.lang.reflect.Proxy;
import java.util.concurrent.TimeUnit;
import java.util.function.Consumer;

import net.zscript.javaclient.addressing.AddressedCommand;
import net.zscript.javaclient.addressing.AddressedResponse;
import net.zscript.javaclient.addressing.ZscriptAddress;
import net.zscript.javaclient.commandPaths.CommandExecutionPath;
import net.zscript.javaclient.commandPaths.ResponseExecutionPath;
import net.zscript.javaclient.sequence.CommandSequence;
import net.zscript.javaclient.sequence.ResponseSequence;
import net.zscript.javaclient.threading.ZscriptWorkerThread;

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

    Connection attach(ZscriptAddress address);

    Connection detach(ZscriptAddress address);

    void send(CommandSequence seq, Consumer<ResponseSequence> callback);

    void send(CommandExecutionPath path, Consumer<ResponseExecutionPath> callback);

    void send(AddressedCommand addr);

    void setNotificationHandler(int notification, Consumer<ResponseSequence> handler);

    void removeNotificationHandler(int notification);
}
