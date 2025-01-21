package net.zscript.javaclient.devicenodes;

import javax.annotation.Nullable;
import java.util.Collection;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.TimeUnit;
import java.util.function.BiConsumer;
import java.util.function.Consumer;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import net.zscript.javaclient.ZscriptClientException;
import net.zscript.javaclient.addressing.AddressedCommand;
import net.zscript.javaclient.addressing.AddressedResponse;
import net.zscript.javaclient.addressing.ZscriptAddress;
import net.zscript.javaclient.commandpaths.CommandExecutionPath;
import net.zscript.javaclient.commandpaths.ResponseExecutionPath;
import net.zscript.javaclient.sequence.CommandSequence;
import net.zscript.javaclient.sequence.ResponseSequence;
import net.zscript.javaclient.threading.ZscriptCallbackThreadpool;

class ZscriptBasicNode implements ZscriptNode {

    private static final Logger LOG = LoggerFactory.getLogger(ZscriptBasicNode.class);

    private final ZscriptCallbackThreadpool callbackPool;

    private final AddressingSystem addressingSystem;

    private final ConnectionBuffer connectionBuffer;
    private final Connection       parentConnection;

    private final EchoAssigner echoSystem;

    private final Map<Integer, Consumer<ResponseSequence>> notificationHandlers = new HashMap<>();

    private final Map<CommandExecutionPath, Consumer<ResponseExecutionPath>> pathCallbacks         = new HashMap<>();
    private final Map<CommandSequence, Consumer<ResponseSequence>>           fullSequenceCallbacks = new HashMap<>();

    private QueuingStrategy strategy = new StandardQueuingStrategy(1000, TimeUnit.SECONDS); // should be enough for almost all cases

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

        parentConnection.onReceive(resp -> {
            try {
                if (resp.hasAddress()) {
                    if (!addressingSystem.response(resp)) {
                        callbackPool.sendCallback(unknownResponseHandler, resp, callbackExceptionHandler);
                    }
                } else {
                    response(resp);
                }
            } catch (Exception e) {
                callbackPool.sendCallback(callbackExceptionHandler, e); // catches all callback exceptions
            }
        });
    }

    public void setStrategy(QueuingStrategy strategy) {
        this.strategy = strategy;
        this.strategy.setBuffer(connectionBuffer);
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

    @Deprecated
    public void send(AddressedCommand addr) {
        strategy.send(addr);
    }

    public void checkTimeouts() {
        Collection<CommandSequence> timedOut = connectionBuffer.checkTimeouts();
        if (!timedOut.isEmpty()) {
            for (CommandSequence seq : timedOut) {
                if (fullSequenceCallbacks.get(seq) != null) {
                    callbackPool.sendCallback(fullSequenceCallbacks.get(seq), ResponseSequence.timedOut(), callbackExceptionHandler);
                } else if (pathCallbacks.get(seq.getExecutionPath()) != null) {
                    callbackPool.sendCallback(pathCallbacks.get(seq.getExecutionPath()), ResponseExecutionPath.blank(), callbackExceptionHandler);
                }
            }
        }
    }

    private void response(AddressedResponse resp) {
        if (resp.getResponseSequence().getResponseFieldValue() != 0) {
            // it's a notification
            final Consumer<ResponseSequence> handler = notificationHandlers.get(resp.getResponseSequence().getResponseFieldValue());
            if (handler != null) {
                callbackPool.sendCallback(handler, resp.getResponseSequence(), callbackExceptionHandler);
            } else {
                callbackPool.sendCallback(unknownNotificationHandler, resp, callbackExceptionHandler);
            }
            return;
        }
        final AddressedCommand found = connectionBuffer.match(resp.getResponseSequence());
        if (found == null) {
            // if it's a recently timed out message, ignore it.
            if (resp.getResponseSequence().hasEchoValue() && echoSystem.unmatchedReceive(resp.getResponseSequence().getEchoValue())) {
                return;
            }
            callbackPool.sendCallback(unknownResponseHandler, resp, callbackExceptionHandler);
            return;
        }

        if (!found.getCommandSequence().getExecutionPath().matchesResponses(resp.getResponseSequence().getExecutionPath())) {
            callbackPool.sendCallback(badCommandResponseMatchHandler, found, resp, callbackExceptionHandler);
            return;
        }

        strategy.mayHaveSpace();
        parentConnection.notifyResponseMatched(found);

        final Consumer<ResponseSequence> seqCallback = fullSequenceCallbacks.remove(found.getCommandSequence());
        if (seqCallback != null) {
            callbackPool.sendCallback(seqCallback, resp.getResponseSequence(), callbackExceptionHandler);
        } else {
            final Consumer<ResponseExecutionPath> pathCallback = pathCallbacks.remove(found.getCommandSequence().getExecutionPath());
            if (pathCallback != null) {
                callbackPool.sendCallback(pathCallback, resp.getResponseSequence().getExecutionPath(), callbackExceptionHandler);
            } else {
                callbackPool.sendCallback(unknownResponseHandler, resp, callbackExceptionHandler);
            }
        }
    }

    public Connection getParentConnection() {
        return parentConnection;
    }

    /**
     * Notification passed up from child to root node to indicate that the supplied command has been matched to a response. Buffers may free records of that command.
     *
     * @param foundCommand the command we've received a response for
     */
    public void responseReceived(AddressedCommand foundCommand) {
        if (connectionBuffer.responseMatched(foundCommand)) {
            strategy.mayHaveSpace();
        }
        parentConnection.notifyResponseMatched(foundCommand);
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

    private BiConsumer<AddressedCommand, AddressedResponse> badCommandResponseMatchHandler = ZscriptBasicNode::defaultBadCommandResponseMatchHandler;
    private Consumer<AddressedResponse>                     unknownResponseHandler         = ZscriptBasicNode::defaultUnknownResponseHandler;
    private Consumer<AddressedResponse>                     unknownNotificationHandler     = ZscriptBasicNode::defaultUnknownNotificationHandler;
    private Consumer<Exception>                             callbackExceptionHandler       = ZscriptBasicNode::defaultCallbackExceptionHandler;

    private static void defaultBadCommandResponseMatchHandler(AddressedCommand cmd, AddressedResponse resp) {
        LOG.error("Command and response do not match: {} ; {}", cmd.getCommandSequence().asStringUtf8(), resp.getResponseSequence().asStringUtf8());
    }

    private static void defaultUnknownResponseHandler(AddressedResponse resp) {
        throw new ZscriptClientException("Unknown response received: " + resp.getResponseSequence().asStringUtf8());
    }

    private static void defaultUnknownNotificationHandler(AddressedResponse resp) {
        LOG.warn("Unknown notification received: {}", resp.getResponseSequence().asStringUtf8());
    }

    private static void defaultCallbackExceptionHandler(Exception e) {
        LOG.error("Exception caught from callback: ", e);
    }

    /**
     * Sets the handler for when a response is received for a command, but where its response sequence doesn't match its corresponding command sequence. This would imply something
     * bad has happened in the Zscript processing on the target device - sounds like a bug!
     *
     * @param badCommandResponseMatchHandler the new handler, or null to restore the default handler
     */
    public void setBadCommandResponseMatchHandler(@Nullable BiConsumer<AddressedCommand, AddressedResponse> badCommandResponseMatchHandler) {
        this.badCommandResponseMatchHandler = badCommandResponseMatchHandler != null ? badCommandResponseMatchHandler : ZscriptBasicNode::defaultBadCommandResponseMatchHandler;
    }

    /**
     * Sets the handler for when a response is received that cannot be related to a known sent command. This would normally mean something weird has happened - it probably
     * shouldn't happen.
     *
     * @param unknownResponseHandler the new handler, or null to restore the default handler
     */
    public void setUnknownResponseHandler(@Nullable Consumer<AddressedResponse> unknownResponseHandler) {
        this.unknownResponseHandler = unknownResponseHandler != null ? unknownResponseHandler : ZscriptBasicNode::defaultUnknownResponseHandler;
    }

    /**
     * Sets the handler for when a notification is received for which no listener is registered. This might be unexpected and worth knowing about, but may easily be ignored.
     *
     * @param unknownNotificationHandler the new handler, or null to restore the default handler
     */
    public void setUnknownNotificationHandler(@Nullable Consumer<AddressedResponse> unknownNotificationHandler) {
        this.unknownNotificationHandler = unknownNotificationHandler != null ? unknownNotificationHandler : ZscriptBasicNode::defaultUnknownNotificationHandler;
    }

    /**
     * Sets the handler for when a response is received for a command, but where its response sequence doesn't match its corresponding command sequence. This would imply something
     * bad has happened in the Zscript processing on the target device - sounds like a bug!
     *
     * @param callbackExceptionHandler the new handler, or null to restore the default handler
     */
    public void setCallbackExceptionHandler(@Nullable Consumer<Exception> callbackExceptionHandler) {
        this.callbackExceptionHandler = callbackExceptionHandler != null ? callbackExceptionHandler : ZscriptBasicNode::defaultCallbackExceptionHandler;
    }
}
