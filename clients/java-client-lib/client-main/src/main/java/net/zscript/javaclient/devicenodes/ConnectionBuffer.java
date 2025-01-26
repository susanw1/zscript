package net.zscript.javaclient.devicenodes;

import java.util.ArrayDeque;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Iterator;
import java.util.List;
import java.util.Queue;
import java.util.concurrent.TimeUnit;

import net.zscript.javaclient.commandpaths.CommandExecutionPath;
import net.zscript.javaclient.sequence.CommandSequence;
import net.zscript.javaclient.sequence.LockCondition;
import net.zscript.javaclient.sequence.ResponseSequence;

public class ConnectionBuffer {
    private final Connection           connection;
    private final Queue<BufferElement> buffer = new ArrayDeque<>();

    private final EchoAssigner echo;

    private int bufferSize;
    private int currentBufferContent = 0;

    private Collection<LockCondition> lockConditions  = new ArrayList<>();
    private boolean                   supports32Locks = false;

    public ConnectionBuffer(Connection connection, EchoAssigner echo, int bufferSize) {
        this.connection = connection;
        this.echo = echo;
        this.bufferSize = bufferSize;
    }

    public void setLockConditions(Collection<LockCondition> lockConditions, boolean supports32Locks) {
        this.lockConditions = lockConditions;
        this.supports32Locks = supports32Locks;
    }

    private void clearOutTo(BufferElement el) {
        for (Iterator<BufferElement> iter = buffer.iterator(); iter.hasNext(); ) {
            BufferElement current = iter.next();
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

    public AddressedCommand match(ResponseSequence sequence) {
        if (sequence.getResponseFieldValue() != 0) {
            throw new IllegalArgumentException("Cannot match notification sequence with command sequence");
        }
        if (!sequence.hasEchoValue()) {
            return null;
        }
        boolean removeUpTo = true;
        for (Iterator<BufferElement> iter = buffer.iterator(); iter.hasNext(); ) {
            BufferElement element = iter.next();
            if (element.isSameLayer() && element.getCommand().getCommandSequence().getEchoValue() == sequence.getEchoValue()) {
                if (!element.getCommand().getCommandSequence().getExecutionPath().matchesResponses(sequence.getExecutionPath())) {
                    return element.getCommand();
                }
                // if the echo value is auto-generated, clear the marker
                echo.responseArrivedNormal(sequence.getEchoValue());
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

    public Collection<CommandSequence> checkTimeouts() {
        final List<CommandSequence> timedOut    = new ArrayList<>(2);
        final long                  currentNano = System.nanoTime();

        for (final Iterator<BufferElement> iter = buffer.iterator(); iter.hasNext(); ) {
            final BufferElement element = iter.next();
            //subtracting first to avoid wrapping issues.
            if (currentNano - element.getNanoTimeTimeout() > 0) {
                if (element.isSameLayer()) {
                    timedOut.add(element.getCommand().getCommandSequence());
                    echo.timeout(element.getCommand().getCommandSequence().getEchoValue());
                }
                iter.remove();
                currentBufferContent -= element.length;
            }
        }
        return timedOut;
    }

    public boolean responseMatched(AddressedCommand cmd) {
        boolean removeUpTo = true;
        for (Iterator<BufferElement> iter = buffer.iterator(); iter.hasNext(); ) {
            BufferElement element = iter.next();
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

    private boolean send(BufferElement element, boolean ignoreLength) {
        if (element.isSameLayer()) {
            int echoVal = element.getCommand().getCommandSequence().getEchoValue();
            for (BufferElement el : buffer) {
                if (el.getCommand().getCommandSequence().getEchoValue() == echoVal) {
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
                echo.manualEchoUse(element.getCommand().getCommandSequence().getEchoValue());
            }
        } else {
            echo.moveEcho();
        }
        buffer.add(element);
        currentBufferContent += element.length;
        connection.send(element.getCommand());
        return true;
    }

    public boolean send(AddressedCommand cmd, boolean ignoreLength, long timeout, TimeUnit unit) {
        return send(new BufferElement(cmd, System.nanoTime() + unit.toNanos(timeout), true), ignoreLength);
    }

    public boolean send(CommandSequence seq, boolean ignoreLength, long timeout, TimeUnit unit) {
        return send(new BufferElement(seq, System.nanoTime() + unit.toNanos(timeout), true), ignoreLength);
    }

    public boolean send(CommandExecutionPath path, boolean ignoreLength, long timeout, TimeUnit unit) {
        final CommandSequence seq = CommandSequence.from(path, echo.getEcho(), supports32Locks, lockConditions);
        return send(new BufferElement(seq, System.nanoTime() + unit.toNanos(timeout), false), ignoreLength);
    }

    public boolean hasNonAddressedInBuffer() {
        for (BufferElement element : buffer) {
            if (element.isSameLayer()) {
                return true;
            }
        }
        return false;
    }

    public int getBufferSize() {
        return bufferSize;
    }

    public int getCurrentBufferContent() {
        return currentBufferContent;
    }

    public void setBufferSize(int bufferSize) {
        this.bufferSize = bufferSize;
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
            this.sameLayer = !cmd.hasAddressLayer();
            this.hadEchoBefore = hadEchoBefore;
            this.length = cmd.getBufferLength();
            this.nanoTimeTimeout = nanoTimeTimeout;
        }

        public boolean isSameLayer() {
            return sameLayer;
        }

        public AddressedCommand getCommand() {
            return cmd;
        }

        public long getNanoTimeTimeout() {
            return nanoTimeTimeout;
        }
    }
}
