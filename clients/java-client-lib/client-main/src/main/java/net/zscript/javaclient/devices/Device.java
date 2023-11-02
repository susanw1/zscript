package net.zscript.javaclient.devices;

import java.util.ArrayDeque;
import java.util.Deque;
import java.util.HashMap;
import java.util.List;
import java.util.ListIterator;
import java.util.Map;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.Future;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.TimeoutException;
import java.util.function.Consumer;

import net.zscript.javaclient.commandbuilder.commandnodes.CommandSequenceNode;
import net.zscript.javaclient.commandbuilder.commandnodes.AndSequenceNode;
import net.zscript.javaclient.commandbuilder.commandnodes.OrSequenceNode;
import net.zscript.javaclient.commandbuilder.commandnodes.ZscriptCommandNode;

import net.zscript.javaclient.commandPaths.Command;
import net.zscript.javaclient.commandPaths.CommandExecutionPath;
import net.zscript.javaclient.commandPaths.ResponseExecutionPath;
import net.zscript.javaclient.commandPaths.ZscriptFieldSet;
import net.zscript.javaclient.commandbuilder.notifications.NotificationHandle;
import net.zscript.javaclient.commandbuilder.notifications.NotificationId;
import net.zscript.javaclient.nodes.ZscriptNode;
import net.zscript.javaclient.sequence.CommandSequence;
import net.zscript.javareceiver.tokenizer.TokenExtendingBuffer;
import net.zscript.javareceiver.tokenizer.Tokenizer;
import net.zscript.model.ZscriptModel;

public class Device {
    private final ZscriptModel model;
    private final ZscriptNode  node;

    private final Map<NotificationId<?>, NotificationHandle> handles = new HashMap<>();

    public Device(ZscriptModel model, ZscriptNode node) {
        this.model = model;
        this.node = node;
    }

    public <T extends NotificationHandle> T getNotificationHandle(NotificationId<T> id) {
        if (handles.get(id) == null) {
            handles.put(id, id.newHandle());
        }
        return id.getHandleType().cast(handles.get(id));
    }

    public <T extends NotificationHandle> void setNotificationListener(NotificationId<T> id, Consumer<NotificationSequenceCallback> listener) {
        node.setNotificationHandler(id.getId(), respSeq -> {
            listener.accept(NotificationSequenceCallback.from(id.newHandle().getSections(), respSeq.getExecutionPath().getResponses()));
        });
    }

    public void sendAsync(final CommandSequenceNode cmdSeq, final Consumer<ResponseSequenceCallback> callback) {
        CommandExecutionTask nodeToPath = convert(cmdSeq, callback);
        node.send(nodeToPath.getPath(), nodeToPath.getCallback());
    }

    public Future<ResponseSequenceCallback> send(final CommandSequenceNode cmdSeq) {
        CompletableFuture<ResponseSequenceCallback> future = new CompletableFuture<>();

        CommandExecutionTask nodeToPath = convert(cmdSeq, future::complete);
        node.send(nodeToPath.getPath(), nodeToPath.getCallback());
        return future;
    }

    public Future<ResponseSequenceCallback> sendExpectSuccess(final CommandSequenceNode cmdSeq) {
        CompletableFuture<ResponseSequenceCallback> future = new CompletableFuture<>();

        CommandExecutionTask nodeToPath = convert(cmdSeq, resp -> {
            List<ResponseSequenceCallback.CommandExecutionSummary<?>> l = resp.getExecutionSummary();
            if (l.get(l.size() - 1).getResponse().succeeded()) {
                future.complete(resp);
            } else {
                future.completeExceptionally(new CommandFailedException(resp));
            }
        });
        node.send(nodeToPath.getPath(), nodeToPath.getCallback());
        return future;
    }

    public ResponseSequenceCallback sendAndWait(final CommandSequenceNode cmdSeq) throws InterruptedException {
        try {
            return send(cmdSeq).get();
        } catch (ExecutionException e) {
            throw new RuntimeException(e.getCause());
        }
    }

    public ResponseSequenceCallback sendAndWaitExpectSuccess(final CommandSequenceNode cmdSeq) throws InterruptedException {
        try {
            return sendExpectSuccess(cmdSeq).get();
        } catch (ExecutionException e) {
            if (e.getCause() instanceof RuntimeException) {
                throw (RuntimeException) e.getCause();
            } else {
                throw new RuntimeException(e.getCause());
            }
        }
    }

    public ResponseSequenceCallback sendAndWait(final CommandSequenceNode cmdSeq, final long timeout, final TimeUnit unit) throws TimeoutException, InterruptedException {
        try {
            return send(cmdSeq).get(timeout, unit);
        } catch (ExecutionException e) {
            throw new RuntimeException(e.getCause());
        }
    }

    public ResponseSequenceCallback sendAndWaitExpectSuccess(final CommandSequenceNode cmdSeq, final long timeout, final TimeUnit unit)
            throws TimeoutException, InterruptedException {
        try {
            return sendExpectSuccess(cmdSeq).get(timeout, unit);
        } catch (ExecutionException e) {
            if (e.getCause() instanceof RuntimeException) {
                throw (RuntimeException) e.getCause();
            } else {
                throw new RuntimeException(e.getCause());
            }
        }
    }

    public void send(final byte[] cmdSeq, final Consumer<byte[]> callback) {
        TokenExtendingBuffer buffer = new TokenExtendingBuffer();
        Tokenizer            tok    = new Tokenizer(buffer.getTokenWriter(), 2);
        for (byte b : cmdSeq) {
            tok.accept(b);
        }
        CommandSequence sequence = CommandSequence.parse(model, buffer.getTokenReader().getFirstReadToken(), false);
        if (sequence.hasEchoField() || sequence.hasLockField()) {
            node.send(sequence, r -> {
                callback.accept(r.toSequence().toByteArray());
            });
        } else {
            node.send(sequence.getExecutionPath(), r -> {
                callback.accept(r.toSequence().toByteArray());
            });
        }
    }

    public static class CommandExecutionTask {
        private final CommandExecutionPath            path;
        private final Consumer<ResponseExecutionPath> callback;

        public CommandExecutionTask(CommandExecutionPath path, Consumer<ResponseExecutionPath> callback) {
            this.path = path;
            this.callback = callback;
        }

        public CommandExecutionPath getPath() {
            return path;
        }

        public Consumer<ResponseExecutionPath> getCallback() {
            return callback;
        }
    }

    CommandExecutionTask convert(CommandSequenceNode cmdSeq, Consumer<ResponseSequenceCallback> callback) {
        class Layer {
            Command success;
            Command failure;
            boolean onSuccessHasOpenParen;
            boolean onSuccessHasCloseParen;

        }
        Map<ZscriptCommandNode<?>, Command> commandMap = new HashMap<>();

        Deque<ListIterator<CommandSequenceNode>> stack = new ArrayDeque<>();
        Deque<CommandSequenceNode>               nodes = new ArrayDeque<>();

        Deque<Layer> destinations = new ArrayDeque<>();
        stack.push(List.of(cmdSeq).listIterator(1));

        Layer first = new Layer();
        first.success = null;
        first.failure = null;
        first.onSuccessHasOpenParen = false;
        destinations.push(first);

        while (true) {
            if (stack.peek().hasPrevious()) {
                Layer               layer = destinations.peek();
                CommandSequenceNode next  = stack.peek().previous();
                if (next instanceof ZscriptCommandNode) {
                    Command cmd = new Command(layer.success, layer.failure, ((ZscriptCommandNode<?>) next).asFieldSet());
                    if (commandMap.containsKey(next)) {
                        throw new IllegalArgumentException(
                                "Repeated use of CommandNode detected - this is not supported. Instead share the builder, and call it twice, or create the commands seperately.");
                    }
                    commandMap.put((ZscriptCommandNode<?>) next, cmd);
                    if (nodes.peek() instanceof OrSequenceNode && stack.peek().hasPrevious()) {
                        layer.failure = cmd;
                    } else {
                        layer.success = cmd;
                        layer.onSuccessHasOpenParen = false;
                        layer.onSuccessHasCloseParen = false;
                    }
                } else if (next instanceof OrSequenceNode) {
                    Layer nextLayer = new Layer();
                    if (nodes.peek() instanceof AndSequenceNode) {
                        if (layer.onSuccessHasCloseParen) {
                            layer.success = new Command(layer.success, layer.failure, ZscriptFieldSet.blank());
                        }
                        layer.onSuccessHasCloseParen = true;
                    }
                    nextLayer.onSuccessHasCloseParen = true;
                    nextLayer.failure = layer.failure;
                    nextLayer.success = layer.success;
                    nextLayer.onSuccessHasOpenParen = false;
                    stack.push(next.getChildren().listIterator(next.getChildren().size()));
                    nodes.push(next);
                    destinations.push(nextLayer);
                } else if (next instanceof AndSequenceNode) {
                    Layer nextLayer = new Layer();
                    nextLayer.failure = layer.failure;
                    nextLayer.success = layer.success;
                    nextLayer.onSuccessHasOpenParen = false;
                    nextLayer.onSuccessHasCloseParen = layer.onSuccessHasCloseParen;
                    stack.push(next.getChildren().listIterator(next.getChildren().size()));
                    nodes.push(next);
                    destinations.push(nextLayer);
                } else {
                    throw new IllegalStateException("Unknown type: " + next);
                }
            } else {
                stack.pop();
                if (nodes.isEmpty()) {
                    break;
                }
                CommandSequenceNode prev      = nodes.pop();
                Layer               prevLayer = destinations.pop();
                Layer               layer     = destinations.peek();
                layer.onSuccessHasOpenParen = prevLayer.onSuccessHasOpenParen;
                if (nodes.peek() instanceof AndSequenceNode) {
                    layer.success = prevLayer.success;
                    if (prev instanceof OrSequenceNode) {
                        if (prevLayer.onSuccessHasOpenParen) {
                            layer.success = new Command(prevLayer.success, prevLayer.failure, ZscriptFieldSet.blank());
                        }
                        layer.onSuccessHasOpenParen = true;
                    }
                } else if (stack.peek().hasPrevious()) {
                    layer.failure = prevLayer.success;
                } else {
                    layer.success = prevLayer.success;
                }
            }
        }
        CommandExecutionPath path = CommandExecutionPath.from(model, destinations.peek().success);
        return new CommandExecutionTask(path, resps -> {
            ResponseSequenceCallback rsCallback = ResponseSequenceCallback.from(path.compareResponses(resps), cmdSeq, commandMap);
            callback.accept(rsCallback);
        });
    }
}
