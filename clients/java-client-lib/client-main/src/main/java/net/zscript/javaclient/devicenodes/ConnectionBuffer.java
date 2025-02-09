package net.zscript.javaclient.devicenodes;

import javax.annotation.Nonnull;
import javax.annotation.Nullable;
import java.util.ArrayDeque;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Iterator;
import java.util.List;
import java.util.concurrent.TimeUnit;

import net.zscript.javaclient.commandpaths.CommandExecutionPath;
import net.zscript.javaclient.sequence.CommandSequence;
import net.zscript.javaclient.sequence.LockCondition;
import net.zscript.javaclient.sequence.ResponseSequence;

/**
 * Stores up command-sequences in a queue, forwarding them to the target by tracking the amount of token buffer space on the target and only sending messages when there is
 * processing space.
 */
public class ConnectionBuffer {
    private final Connection                connection;
    private final ArrayDeque<BufferElement> sentList = new ArrayDeque<>();

    private final EchoAssigner echoAssigner;
    private final TimeSource   timeSource;

    /** Estimate size of this device's buffer, for fullness estimation - may be changed if true buffer-size is discovered later. */
    private int bufferSize;
    /** Keeps track of the number of bytes currently in this device's buffer to prevent overflows. */
    private int currentBufferContent = 0;

    private Collection<LockCondition> lockConditions  = List.of();
    private boolean                   supports32Locks = false;

    public ConnectionBuffer(Connection connection, EchoAssigner echoAssigner, int bufferSize) {
        this(connection, echoAssigner, bufferSize, System::nanoTime);
    }

    ConnectionBuffer(Connection connection, EchoAssigner echoAssigner, int bufferSize, TimeSource timeSource) {
        this.connection = connection;
        this.echoAssigner = echoAssigner;
        this.bufferSize = bufferSize;
        this.timeSource = timeSource;
    }

    public boolean forward(AddressedCommand cmd, boolean ignoreLength, long timeout, TimeUnit unit) {
        return addBufferElementToBuffer(new BufferElement(cmd, timeSource.nanoTime() + unit.toNanos(timeout), cmd.getCommandSequence().hasEchoField()), ignoreLength);
    }

    /**
     * Queues the supplied command sequence to be sent. The sequence optionally contains an echo field, otherwise one will be added. The message may be refused if a supplied echo
     * value is already in use, or if the command buffer on the corresponding device would become full.
     *
     * @param sequence     the command sequence to send
     * @param ignoreLength set to true to enqueue regardless of the buffer considerations
     * @param timeout      how long before command sequence is consider "late"
     * @param unit         the units for the timeout
     * @return true if successfully enqueued, false otherwise
     */
    public boolean send(CommandSequence sequence, boolean ignoreLength, long timeout, TimeUnit unit) {
        return addBufferElementToBuffer(new BufferElement(sequence, timeSource.nanoTime() + unit.toNanos(timeout), sequence.hasEchoField()), ignoreLength);
    }

    public boolean send(CommandExecutionPath path, boolean ignoreLength, long timeout, TimeUnit unit) {
        final CommandSequence seq = CommandSequence.from(path, echoAssigner.getEcho(), supports32Locks, lockConditions);
        return addBufferElementToBuffer(new BufferElement(seq, timeSource.nanoTime() + unit.toNanos(timeout), false), ignoreLength);
    }

    /**
     * Takes a wrapped AddressedCommand, ensures it's not an echo-duplicate and that it will fit in the target's buffer, then buffers it (so we know when we've removed it) and
     * totals the estimated buffer usage for this device.
     *
     * @param element      the command to buffer
     * @param ignoreLength true if it is ok to ignore the buffer length requirement, false otherwise (normal)
     * @return true if sent, false if rejected (buffer full, echo already assigned)
     */
    private boolean addBufferElementToBuffer(BufferElement element, boolean ignoreLength) {
        if (element.isSameLayer()) {
            final int echoVal = element.getEchoValue();
            for (final BufferElement el : sentList) {
                if (el.getEchoValue() == echoVal) {
                    return false;
                }
            }
        }
        if (!ignoreLength && element.length + currentBufferContent >= bufferSize) {
            return false;
        }
        // make sure echo system knows about echo usage...
        if (element.hadEchoBefore) {
            if (element.isSameLayer()) {
                echoAssigner.manualEchoUse(element.getEchoValue());
            }
        } else {
            echoAssigner.moveEcho();
        }
        sentList.add(element);
        currentBufferContent += element.length;
        connection.send(element.getCommand());
        return true;
    }

    /**
     * Removes elements in the queue, starting from 0 up to (and including) the supplied element.
     *
     * @param el the element to match
     */
    private void clearOutTo(BufferElement el) {
        for (final Iterator<BufferElement> iter = sentList.iterator(); iter.hasNext(); ) {
            final BufferElement current = iter.next();
            if (current == el) {
                iter.remove();
                currentBufferContent -= current.length;
                return;
            } else if (!current.isSameLayer()) {
                iter.remove();
                currentBufferContent -= current.length;
            }
        }
    }

    @Nullable
    public AddressedCommand match(ResponseSequence responseSequence) {
        if (responseSequence.getResponseFieldValue() > 0) {
            throw new IllegalArgumentException("Cannot match notification sequence with command sequence");
        }
        if (!responseSequence.hasEchoValue()) {
            return null;
        }
        boolean removeUpTo = true;
        for (Iterator<BufferElement> iter = sentList.iterator(); iter.hasNext(); ) {
            final BufferElement element = iter.next();
            // re-jig this logic a little...extract element.isSameLayer() condition
            if (element.isSameLayer() && element.getEchoValue() == responseSequence.getEchoValue()) {
                if (!element.getCommand().getCommandSequence().getExecutionPath().matchesResponses(responseSequence.getExecutionPath())) {
                    // confused here. It had right echo, but didn't match, right?
                    return element.getCommand();
                }
                // if the echo value is auto-generated, clear the marker
                echoAssigner.responseArrivedNormal(responseSequence.getEchoValue());
                if (removeUpTo) {
                    clearOutTo(element);
                } else {
                    iter.remove();
                    currentBufferContent -= element.length;
                }
                return element.getCommand();
            } else if (element.isSameLayer()) {
                removeUpTo = false;
            }
        }
        return null;
    }

    @Nonnull
    public Collection<CommandSequence> checkTimeouts() {
        final List<CommandSequence> timedOut    = new ArrayList<>(2);
        final long                  currentNano = timeSource.nanoTime();

        for (final Iterator<BufferElement> iter = sentList.iterator(); iter.hasNext(); ) {
            final BufferElement element = iter.next();
            //subtracting first to avoid wrapping issues.
            if (currentNano - element.getNanoTimeTimeout() > 0) {
                if (element.isSameLayer()) {
                    timedOut.add(element.getCommand().getCommandSequence());
                    echoAssigner.timeout(element.getEchoValue());
                }
                iter.remove();
                currentBufferContent -= element.length;
            }
        }
        return timedOut;
    }

    /**
     * If the supplied cmd has been matched by a sub-node, then it should be removed from the sent-list. In addition, any earlier forwarded (addressed) sub-node commands are
     * removed too, because if _this_ command was executed, its predecessors can no longer be in the buffer using up space. Note that "removal" in this sense is important only in
     * order to keep the currentBufferContent fullness estimate accurate.
     *
     * @param cmd the command to be removed
     * @return true if any removed, false if buffer is unchanged (cmd wasn't found)
     */
    public boolean responseMatchedBySubnode(AddressedCommand cmd) {
        boolean removeUpTo = true;
        for (final Iterator<BufferElement> iter = sentList.iterator(); iter.hasNext(); ) {
            final BufferElement element = iter.next();
            if (element.getCommand() == cmd) {
                if (removeUpTo) {
                    clearOutTo(element);
                } else {
                    iter.remove();
                    currentBufferContent -= element.length;
                }
                return true;
            } else if (element.isSameLayer()) {
                removeUpTo = false;
            }
        }
        return false;
    }

    /**
     * Determines whether all the messages are forwarded from sub-devices, rather than actually targeting this one.
     *
     * @return true if all forwarded, false if any are actually targeted to this device (or if empty)
     */
    public boolean hasNonAddressedInBuffer() {
        for (final BufferElement element : sentList) {
            if (element.isSameLayer()) {
                return true;
            }
        }
        return false;
    }

    public void setLockConditions(Collection<LockCondition> lockConditions, boolean supports32Locks) {
        this.lockConditions = lockConditions;
        this.supports32Locks = supports32Locks;
    }

    /**
     * Determines the number of outgoing messages currently queued for sending.
     *
     * @return the current number of queued messages
     */
    public int getQueueLength() {
        return sentList.size();
    }

    /**
     * For testing: return the nth item in the queue. Not efficient.
     *
     * @return the nth queue item
     */
    AddressedCommand getQueueItem(int n) {
        return List.copyOf(sentList).get(n).getCommand();
    }

    /**
     * Gets the presumed size of the token buffer on the target device, which allows us to determine the maximum number of token bytes that can be sent without triggering buffer
     * overflow. ("presumed" means this is the size we've been told to use, which might have involved communicating with the device, or might just be a default or estimate).
     *
     * @return presumed size of the token buffer on the target
     */
    public int getBufferSize() {
        return bufferSize;
    }

    public void setBufferSize(int bufferSize) {
        this.bufferSize = bufferSize;
    }

    /**
     * Indicates how much of the target's token buffer is in actual use, based on what has been sent and what has come back.
     *
     * @return estimated currrent buffer content size
     */
    public int getCurrentBufferContent() {
        return currentBufferContent;
    }

    private static class BufferElement {
        private final AddressedCommand cmd;
        private final boolean          sameLayer;
        private final boolean          hadEchoBefore;
        private final int              length;
        private final long             nanoTimeTimeout;

        BufferElement(CommandSequence seq, long nanoTimeTimeout, boolean hadEchoBefore) {
            this(new AddressedCommand(seq), nanoTimeTimeout, hadEchoBefore);
        }

        BufferElement(AddressedCommand cmd, long nanoTimeTimeout, boolean hadEchoBefore) {
            this.cmd = cmd;
            this.sameLayer = !cmd.hasAddress();
            this.hadEchoBefore = hadEchoBefore;
            this.length = cmd.getBufferLength();
            this.nanoTimeTimeout = nanoTimeTimeout;
        }

        public boolean isSameLayer() {
            return sameLayer;
        }

        public boolean isForwarded() {
            return !sameLayer;
        }

        @Nonnull
        public AddressedCommand getCommand() {
            return cmd;
        }

        public long getNanoTimeTimeout() {
            return nanoTimeTimeout;
        }

        public int getEchoValue() {
            return cmd.getCommandSequence().getEchoValue();
        }
    }

    /**
     * Abstraction of System.nanoTime() to allow time-injection for testing.
     */
    @FunctionalInterface
    interface TimeSource {
        long nanoTime();
    }
}
