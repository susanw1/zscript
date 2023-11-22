package net.zscript.javaclient.nodes;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.Collection;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.TimeUnit;
import java.util.function.Consumer;

import net.zscript.javaclient.addressing.AddressedCommand;
import net.zscript.javaclient.addressing.AddressedResponse;
import net.zscript.javaclient.addressing.ZscriptAddress;
import net.zscript.javaclient.commandPaths.CommandExecutionPath;
import net.zscript.javaclient.commandPaths.ResponseExecutionPath;
import net.zscript.javaclient.sequence.CommandSequence;
import net.zscript.javaclient.sequence.ResponseSequence;
import net.zscript.javaclient.threading.ZscriptCallbackThreadpool;

class ZscriptBasicNode implements ZscriptNode {

    private static final Logger LOG = LoggerFactory.getLogger(ZscriptBasicNode.class);

    private final ZscriptCallbackThreadpool callbackPool;

    private final AddressingSystem addressingSystem;

    private final ConnectionBuffer connectionBuffer;
    private final Connection       parentConnection;

    private QueuingStrategy strategy = new StandardQueuingStrategy(1000, TimeUnit.MILLISECONDS); // should be enough for almost all cases

    private Consumer<AddressedResponse> unknownResponseHandler = r -> {
        throw new IllegalStateException("Unknown response received: " + r);
    };

    private Consumer<Exception> callbackExceptionHandler = e -> {
        LOG.error(e.getMessage());
    };

    private final Map<Integer, Consumer<ResponseSequence>> notificationHandlers = new HashMap<>();

    private final Map<CommandExecutionPath, Consumer<ResponseExecutionPath>> pathCallbacks         = new HashMap<>();
    private final Map<CommandSequence, Consumer<ResponseSequence>>           fullSequenceCallbacks = new HashMap<>();

    private final EchoAssigner echoSystem;

    ZscriptBasicNode(ZscriptCallbackThreadpool callbackPool, Connection parentConnection, int bufferSize) {
        this(callbackPool, parentConnection, bufferSize, 100, TimeUnit.MILLISECONDS);
    }

    ZscriptBasicNode(ZscriptCallbackThreadpool callbackPool, Connection parentConnection, int bufferSize, long minSegmentChangeTime, TimeUnit unit) {
        this.callbackPool = callbackPool;
        this.addressingSystem = new AddressingSystem(this);
        this.parentConnection = parentConnection;
        this.echoSystem = new EchoAssigner(unit.toNanos(minSegmentChangeTime));
        this.connectionBuffer = new ConnectionBuffer(parentConnection, echoSystem, bufferSize);
        this.strategy.setBuffer(connectionBuffer);
        parentConnection.onReceive(r -> {
            try {
                if (r.hasAddress()) {
                    if (!addressingSystem.response(r)) {
                        callbackPool.sendCallback(unknownResponseHandler, r);
                    }
                } else {
                    response(r);
                }
            } catch (Exception e) {
                callbackPool.sendCallback(callbackExceptionHandler, e); // catches all callback exceptions
            }
        });
    }

    public void setUnknownResponseHandler(Consumer<AddressedResponse> unknownResponseHandler) {
        this.unknownResponseHandler = unknownResponseHandler;
    }

    public void setStrategy(QueuingStrategy strategy) {
        this.strategy = strategy;
        this.strategy.setBuffer(connectionBuffer);
    }

    public void setCallbackExceptionHandler(Consumer<Exception> callbackExceptionHandler) {
        this.callbackExceptionHandler = callbackExceptionHandler;
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

    public void checkTimeouts() {
        Collection<CommandSequence> timedOut = connectionBuffer.checkTimeouts();
        if (!timedOut.isEmpty()) {
            long nanoTime = System.nanoTime();
            for (CommandSequence seq : timedOut) {
                if (fullSequenceCallbacks.get(seq) != null) {
                    callbackPool.sendCallback(fullSequenceCallbacks.get(seq), ResponseSequence.blank());
                } else if (pathCallbacks.get(seq.getExecutionPath()) != null) {
                    callbackPool.sendCallback(pathCallbacks.get(seq.getExecutionPath()), ResponseExecutionPath.blank());
                }
            }
        }
    }

    private void response(AddressedResponse resp) {
        if (resp.getContent().getResponseValue() != 0) {
            Consumer<ResponseSequence> handler = notificationHandlers.get(resp.getContent().getResponseValue());
            if (handler != null) {
                callbackPool.sendCallback(handler, resp.getContent());
            } else {
                callbackPool.sendCallback(unknownResponseHandler, resp);
            }
            return;
        }
        AddressedCommand found = connectionBuffer.match(resp.getContent());
        if (found == null) {
            // if it's a recently timed out message, ignore it.
            if (resp.getContent().hasEchoValue() && echoSystem.unmatchedReceive(resp.getContent().getEchoValue())) {
                return;
            }
            callbackPool.sendCallback(unknownResponseHandler, resp);
            return;
        }
        strategy.mayHaveSpace();
        parentConnection.responseReceived(found);
        Consumer<ResponseSequence> seqCallback = fullSequenceCallbacks.remove(found.getContent());
        if (seqCallback != null) {
            callbackPool.sendCallback(seqCallback, resp.getContent());
        } else {
            Consumer<ResponseExecutionPath> pathCallback = pathCallbacks.remove(found.getContent().getExecutionPath());
            if (pathCallback != null) {
                callbackPool.sendCallback(pathCallback, resp.getContent().getExecutionPath());
            } else {
                callbackPool.sendCallback(unknownResponseHandler, resp);
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

    @Override
    public void setBufferSize(int bufferSize) {
        connectionBuffer.setBufferSize(bufferSize);
    }
}
