package net.zscript.javaclient.nodes;

import java.util.ArrayDeque;
import java.util.Collection;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.Queue;
import java.util.concurrent.TimeUnit;
import java.util.function.Consumer;

import net.zscript.javaclient.addressing.AddressedCommand;
import net.zscript.javaclient.addressing.AddressedResponse;
import net.zscript.javaclient.addressing.ZscriptAddress;
import net.zscript.javaclient.commandPaths.CommandExecutionPath;
import net.zscript.javaclient.commandPaths.ResponseExecutionPath;
import net.zscript.javaclient.sequence.CommandSequence;
import net.zscript.javaclient.sequence.ResponseSequence;

public class ZscriptNode {

    private static class CommandTimeout {
        private final int  echo;
        private final long timeoutNanoTime;

        private CommandTimeout(int echo, long timeoutNanoTime) {
            this.echo = echo;
            this.timeoutNanoTime = timeoutNanoTime;
        }

        public int getEcho() {
            return echo;
        }

        public long getTimeoutNanoTime() {
            return timeoutNanoTime;
        }
    }

    private final AddressingSystem addressingSystem;

    private final ConnectionBuffer connectionBuffer;
    private final Connection       parentConnection;

    private QueuingStrategy strategy = new StandardQueuingStrategy(1000, TimeUnit.MILLISECONDS); // should be enough for almost all cases

    private Consumer<AddressedResponse> unknownResponseHandler = r -> {
        throw new IllegalStateException("Unknown response received: " + r);
    };

    private Consumer<Exception> callbackExceptionHandler = r -> {
    };

    private final Map<Integer, Consumer<ResponseSequence>> notificationHandlers = new HashMap<>();

    private final Map<CommandExecutionPath, Consumer<ResponseExecutionPath>> pathCallbacks         = new HashMap<>();
    private final Map<CommandSequence, Consumer<ResponseSequence>>           fullSequenceCallbacks = new HashMap<>();

    private final Queue<CommandTimeout> timeouts = new ArrayDeque<>();
    // How many timed-out commands are retained to avoid meaningless errors
    private final int                   savedTimeoutCount;
    // If the timeout queue wraps in less than this time, too many timeouts are occurring.
    private final long                  timeoutRollTooSoonCount;

    public ZscriptNode(Connection parentConnection) {
        this(parentConnection, 128, 100, 100, TimeUnit.MILLISECONDS);
    }

    public ZscriptNode(Connection parentConnection, int bufferSize, int savedTimeoutCount, long timeoutRollTooSoon, TimeUnit unit) {
        if (savedTimeoutCount < 0) {
            throw new IllegalArgumentException("Saved timeout count cannot be negative");
        }
        this.savedTimeoutCount = savedTimeoutCount;
        this.timeoutRollTooSoonCount = unit.toNanos(timeoutRollTooSoon);
        this.addressingSystem = new AddressingSystem(this);
        this.parentConnection = parentConnection;
        this.connectionBuffer = new ConnectionBuffer(parentConnection, bufferSize);
        this.strategy.setBuffer(connectionBuffer);
        parentConnection.onReceive(r -> {
            try {
                if (r.hasAddress()) {
                    if (!addressingSystem.response(r)) {
                        unknownResponseHandler.accept(r);
                    }
                } else {
                    response(r);
                }
            } catch (Exception e) {
                callbackExceptionHandler.accept(e); // catches all callback exceptions
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
        if (seq.hasEchoField()) {
            for (Iterator<CommandTimeout> iter = timeouts.iterator(); iter.hasNext(); ) {
                int echo = iter.next().getEcho();
                if (echo == seq.getEchoValue()) {
                    iter.remove();
                    //TODO: log: time since last use timed out.
                }
            }
        } else {
            checkTimeoutEchoReuse();
        }
    }

    public void send(CommandExecutionPath path, Consumer<ResponseExecutionPath> callback) {
        pathCallbacks.put(path, callback);
        strategy.send(path);
        checkTimeoutEchoReuse();
    }

    public void send(AddressedCommand addr) {
        strategy.send(addr);
    }

    //TODO: add call to execute this - use worker thread
    private void checkTimeouts() {
        Collection<CommandSequence> timedOut = connectionBuffer.checkTimeouts();
        if (!timedOut.isEmpty()) {
            long nanoTime = System.nanoTime();
            for (CommandSequence cmd : timedOut) {
                timeouts.add(new CommandTimeout(cmd.getEchoValue(), nanoTime));
            }
            while (timeouts.size() > savedTimeoutCount) {
                long diff = nanoTime - timeouts.poll().getTimeoutNanoTime();
                if (diff < timeoutRollTooSoonCount) {
                    //log: too many timeouts...
                }
            }
        }
    }

    private void checkTimeoutEchoReuse() {
        for (Iterator<CommandTimeout> iter = timeouts.iterator(); iter.hasNext(); ) {
            int echo = iter.next().getEcho();
            if (connectionBuffer.isApproachingEcho(echo)) {
                iter.remove();
                //TODO: log: time since last use timed out.
            }
        }
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
        checkTimeoutEchoReuse();
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
            checkTimeoutEchoReuse();
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
