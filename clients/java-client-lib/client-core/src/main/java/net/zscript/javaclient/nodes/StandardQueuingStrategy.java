package net.zscript.javaclient.nodes;

import java.util.ArrayDeque;
import java.util.Queue;
import java.util.concurrent.TimeUnit;

import net.zscript.javaclient.addressing.AddressedCommand;
import net.zscript.javaclient.commandpaths.CommandExecutionPath;
import net.zscript.javaclient.sequence.CommandSequence;

public class StandardQueuingStrategy implements QueuingStrategy {

    private final Queue<QueueElement> waiting = new ArrayDeque<>();

    private ConnectionBuffer buffer;

    private final long     timeout;
    private final TimeUnit unit;

    public StandardQueuingStrategy(long timeout, TimeUnit unit) {
        this.timeout = timeout;
        this.unit = unit;
    }

    public void setBuffer(ConnectionBuffer buffer) {
        this.buffer = buffer;
    }

    @Override
    public void mayHaveSpace() {
        while (!waiting.isEmpty() && waiting.peek().addToBuffer(false)) {
            waiting.poll();
        }
        if (!waiting.isEmpty() && !buffer.hasNonAddressedInBuffer()) {
            buffer.send(CommandExecutionPath.blank(), true, timeout, unit);
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

    /**
     * Defines an item that can be queued for sending.
     */
    private interface QueueElement {
        boolean addToBuffer(boolean ignoreLength);
    }

    /**
     * Supports queuing command sequences that already may have echo tracking.
     */
    private class SequenceQueueElement implements QueueElement {
        private final CommandSequence seq;

        private SequenceQueueElement(CommandSequence seq) {
            this.seq = seq;
        }

        @Override
        public boolean addToBuffer(boolean ignoreLength) {
            return buffer.send(seq, ignoreLength, timeout, unit);
        }
    }

    /**
     * Supports queuing command sequences that don't have echo tracking (yet).
     */
    private class PathQueueElement implements QueueElement {
        private final CommandExecutionPath path;

        private PathQueueElement(CommandExecutionPath path) {
            this.path = path;
        }

        @Override
        public boolean addToBuffer(boolean ignoreLength) {
            return buffer.send(path, ignoreLength, timeout, unit);
        }
    }

    /**
     * Supports queuing command sequences that have an address.
     */
    private class AddressedQueueElement implements QueueElement {
        private final AddressedCommand addr;

        private AddressedQueueElement(AddressedCommand addr) {
            this.addr = addr;
        }

        @Override
        public boolean addToBuffer(boolean ignoreLength) {
            return buffer.send(addr, ignoreLength, timeout, unit);
        }
    }
}
