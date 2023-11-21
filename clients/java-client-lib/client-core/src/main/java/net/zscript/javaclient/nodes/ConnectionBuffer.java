package net.zscript.javaclient.nodes;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.ArrayDeque;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Iterator;
import java.util.List;
import java.util.Queue;
import java.util.concurrent.TimeUnit;

import net.zscript.javaclient.addressing.AddressedCommand;
import net.zscript.javaclient.commandPaths.CommandExecutionPath;
import net.zscript.javaclient.sequence.CommandSequence;
import net.zscript.javaclient.sequence.LockCondition;
import net.zscript.javaclient.sequence.ResponseSequence;

public class ConnectionBuffer {

    class BufferElement {
        private final AddressedCommand cmd;
        private final boolean          sameLayer;
        private final boolean          hadEchoBefore;
        private final int              length;
        private final long             nanoTimeTimeout;

        BufferElement(CommandSequence seq, long nanoTimeTimeout) {
            this.cmd = new AddressedCommand(seq);
            this.sameLayer = true;
            this.hadEchoBefore = true;
            this.length = seq.getBufferLength();
            this.nanoTimeTimeout = nanoTimeTimeout;
        }

        BufferElement(CommandExecutionPath cmd, long nanoTimeTimeout) {
            CommandSequence seq = CommandSequence.from(cmd, echo.getEcho(), supports32Locks, lockConditions);
            this.cmd = new AddressedCommand(seq);
            this.sameLayer = true;
            this.hadEchoBefore = false;
            this.length = seq.getBufferLength();
            this.nanoTimeTimeout = nanoTimeTimeout;
        }

        BufferElement(AddressedCommand cmd, long nanoTimeTimeout) {
            this.cmd = cmd;
            this.sameLayer = !cmd.hasAddressLayer();
            this.hadEchoBefore = true;
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
        if (sequence.getResponseValue() != 0) {
            throw new IllegalArgumentException("Cannot match notification sequence with command sequence");
        }
        if (!sequence.hasEchoValue()) {
            return null;
        }
        boolean removeUpTo = true;
        for (Iterator<BufferElement> iter = buffer.iterator(); iter.hasNext(); ) {
            BufferElement element = iter.next();
            if (element.isSameLayer() && element.getCommand().getContent().getEchoValue() == sequence.getEchoValue()) {
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
        List<CommandSequence> timedOut    = new ArrayList<>(2);
        long                  currentNano = System.nanoTime();
        for (Iterator<BufferElement> iter = buffer.iterator(); iter.hasNext(); ) {
            BufferElement element = iter.next();
            //subtracting first to avoid wrapping issues.
            if (currentNano - element.getNanoTimeTimeout() > 0) {
                if (element.isSameLayer()) {
                    timedOut.add(element.getCommand().getContent());
                    echo.timeout(element.getCommand().getContent().getEchoValue());
                }
                iter.remove();
                currentBufferContent -= element.length;
            }
        }
        return timedOut;
    }

    public boolean responseReceived(AddressedCommand cmd) {
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
            int echoVal = element.getCommand().getContent().getEchoValue();
            for (BufferElement el : buffer) {
                if (el.getCommand().getContent().getEchoValue() == echoVal) {
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
                echo.manualEchoUse(element.getCommand().getContent().getEchoValue());
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
        return send(new BufferElement(cmd, System.nanoTime() + unit.toNanos(timeout)), ignoreLength);
    }

    public boolean send(CommandSequence seq, boolean ignoreLength, long timeout, TimeUnit unit) {
        return send(new BufferElement(seq, System.nanoTime() + unit.toNanos(timeout)), ignoreLength);
    }

    public boolean send(CommandExecutionPath path, boolean ignoreLength, long timeout, TimeUnit unit) {
        return send(new BufferElement(path, System.nanoTime() + unit.toNanos(timeout)), ignoreLength);
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
}
