package net.zscript.javaclient.devicenodes;

import java.util.ArrayDeque;
import java.util.Queue;
import java.util.concurrent.TimeUnit;

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
    public void bufferSpaceFreed() {
        // Use the estimated token-buffer length to decide whether to feed more messages from the waiting queue up into the ConnectionBuffer and beyond
        while (!waiting.isEmpty() && waiting.peek().addToBuffer(false)) {
            waiting.poll();
        }

        // If buffer only has "addressed" items (ie it's all routing), we can only know the real device has
        // passed them on when they've gone through and been answered. This means we don't know how full the
        // device's buffer is. Let's send a tiny dummy Blank command to elicit an immediate echo-valued response.
        if (!waiting.isEmpty() && !buffer.hasNonAddressedInBuffer()) {
            buffer.send(CommandExecutionPath.blank(), true, timeout, unit);
        }
    }

    private void send(QueueElement el) {
        boolean wasEmpty = waiting.isEmpty();
        waiting.add(el);
        if (wasEmpty) {
            bufferSpaceFreed();
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
    public void forward(AddressedCommand addr) {
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
            return buffer.forward(addr, ignoreLength, timeout, unit);
        }
    }
}
