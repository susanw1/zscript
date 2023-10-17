package net.zscript.javaclient.nodes;

import java.util.HashMap;
import java.util.Map;
import java.util.function.Consumer;

import net.zscript.javaclient.addressing.AddressedCommand;
import net.zscript.javaclient.addressing.AddressedResponse;
import net.zscript.javaclient.addressing.ZscriptAddress;
import net.zscript.javaclient.commandPaths.CommandExecutionPath;
import net.zscript.javaclient.commandPaths.ResponseExecutionPath;
import net.zscript.javaclient.sequence.CommandSequence;
import net.zscript.javaclient.sequence.ResponseSequence;

public class ZscriptNode {
    private final AddressingSystem addressingSystem;

    private final ConnectionBuffer connectionBuffer;
    private final Connection       parentConnection;

    private QueuingStrategy strategy = new StandardQueuingStrategy();

    private Consumer<AddressedResponse> unknownResponseHandler = r -> {
        throw new IllegalStateException("Unknown response received: " + r);
    };

    private final Map<Integer, Consumer<ResponseSequence>> notificationHandlers = new HashMap<>();

    private final Map<CommandExecutionPath, Consumer<ResponseExecutionPath>> pathCallbacks         = new HashMap<>();
    private final Map<CommandSequence, Consumer<ResponseSequence>>           fullSequenceCallbacks = new HashMap<>();

    public ZscriptNode(Connection parentConnection, int bufferSize) {
        this.addressingSystem = new AddressingSystem(this);
        this.parentConnection = parentConnection;
        this.connectionBuffer = new ConnectionBuffer(parentConnection, bufferSize);
        parentConnection.onReceive(r -> {
            if (r.hasAddress()) {
                if (!addressingSystem.response(r)) {
                    unknownResponseHandler.accept(r);
                }
            } else {
                response(r);
            }
        });
    }

    public void setUnknownResponseHandler(Consumer<AddressedResponse> unknownResponseHandler) {
        this.unknownResponseHandler = unknownResponseHandler;
    }

    public void setStrategy(QueuingStrategy strategy) {
        this.strategy = strategy;
    }

    public Connection attach(ZscriptAddress address) {
        return addressingSystem.attach(address);
    }

    public Connection detach(ZscriptAddress address) {
        return addressingSystem.detach(address);
    }

    public void send(CommandSequence seq, Consumer<ResponseSequence> callback) {
        fullSequenceCallbacks.put(seq, callback);
        strategy.send(seq);
    }

    public void send(CommandExecutionPath path, Consumer<ResponseExecutionPath> callback) {
        pathCallbacks.put(path, callback);
        strategy.send(path);
    }

    public void send(AddressedCommand addr) {
        strategy.send(addr);
    }

    private void response(AddressedResponse resp) {
        if (resp.getContent().getResponseValue() != 0) {
            Consumer<ResponseSequence> handler = notificationHandlers.get(resp.getContent().getResponseValue());
            if (handler != null) {
                handler.accept(resp.getContent());
            } else {
                unknownResponseHandler.accept(resp);
            }
            return;
        }
        AddressedCommand found = connectionBuffer.match(resp.getContent());
        if (found == null) {
            unknownResponseHandler.accept(resp);
            return;
        }
        strategy.mayHaveSpace();
        parentConnection.responseReceived(found);
        Consumer<ResponseSequence> seqCallback = fullSequenceCallbacks.remove(found.getContent());
        if (seqCallback != null) {
            seqCallback.accept(resp.getContent());
        } else {
            Consumer<ResponseExecutionPath> pathCallback = pathCallbacks.remove(found.getContent().getExecutionPath());
            if (pathCallback != null) {
                pathCallback.accept(resp.getContent().getExecutionPath());
            } else {
                unknownResponseHandler.accept(resp);
            }
        }

    }

    public Connection getParentConnection() {
        return parentConnection;
    }

    public void responseReceived(AddressedCommand found) {
        if (connectionBuffer.responseReceived(found)) {
            strategy.mayHaveSpace();
        }
        parentConnection.responseReceived(found);
    }

    public void setNotificationHandler(int notification, Consumer<ResponseSequence> handler) {
        notificationHandlers.put(notification, handler);
    }

    public void removeNotificationHandler(int notification) {
        notificationHandlers.remove(notification);
    }
}
