package net.zscript.javaclient.nodes;

import java.util.ArrayDeque;
import java.util.Queue;

import net.zscript.javaclient.addressing.AddressedCommand;
import net.zscript.javaclient.commandPaths.CommandExecutionPath;
import net.zscript.javaclient.sequence.CommandSequence;

public class StandardQueuingStrategy implements QueuingStrategy {

    private interface QueueElement {
        boolean addToBuffer(boolean ignoreLength);
    }

    private class SequenceQueueElement implements QueueElement {
        private final CommandSequence seq;

        private SequenceQueueElement(CommandSequence seq) {
            this.seq = seq;
        }

        @Override
        public boolean addToBuffer(boolean ignoreLength) {
            return buffer.send(seq, ignoreLength);
        }
    }

    private class PathQueueElement implements QueueElement {
        private final CommandExecutionPath path;

        private PathQueueElement(CommandExecutionPath path) {
            this.path = path;
        }

        @Override
        public boolean addToBuffer(boolean ignoreLength) {
            return buffer.send(path, ignoreLength);
        }
    }

    private class AddressedQueueElement implements QueueElement {
        private final AddressedCommand addr;

        private AddressedQueueElement(AddressedCommand addr) {
            this.addr = addr;
        }

        @Override
        public boolean addToBuffer(boolean ignoreLength) {
            return buffer.send(addr, ignoreLength);
        }
    }

    private final Queue<QueueElement> waiting = new ArrayDeque<>();

    private ConnectionBuffer buffer;

    public void setBuffer(ConnectionBuffer buffer) {
        this.buffer = buffer;
    }

    @Override
    public void mayHaveSpace() {
        while (!waiting.isEmpty() && waiting.peek().addToBuffer(false)) {
            waiting.poll();
        }
        if (!waiting.isEmpty() && !buffer.hasNonAddressedInBuffer()) {
            buffer.send(CommandExecutionPath.blank(), true);
        }
    }

    private void send(QueueElement el) {
        boolean wasEmpty = waiting.isEmpty();
        waiting.add(el);
        if (wasEmpty) {
            mayHaveSpace();
        }
    }

    @Override
    public void send(CommandSequence seq) {
        send(new SequenceQueueElement(seq));
    }

    @Override
    public void send(CommandExecutionPath path) {
        send(new PathQueueElement(path));
    }

    @Override
    public void send(AddressedCommand addr) {
        send(new AddressedQueueElement(addr));
    }
}
