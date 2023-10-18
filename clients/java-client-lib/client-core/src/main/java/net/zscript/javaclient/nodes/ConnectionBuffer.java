package net.zscript.javaclient.nodes;

import java.util.ArrayDeque;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Iterator;
import java.util.Queue;

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

        BufferElement(CommandSequence seq) {
            this.cmd = new AddressedCommand(seq);
            this.sameLayer = true;
            this.hadEchoBefore = true;
            this.length = seq.getBufferLength();
        }

        BufferElement(CommandExecutionPath cmd) {
            CommandSequence seq = CommandSequence.from(cmd, currentEcho, supports32Locks, lockConditions);
            this.cmd = new AddressedCommand(seq);
            this.sameLayer = true;
            this.hadEchoBefore = false;
            this.length = seq.getBufferLength();
        }

        BufferElement(AddressedCommand cmd) {
            this.cmd = cmd;
            this.sameLayer = !cmd.hasAddressLayer();
            this.hadEchoBefore = true;
            this.length = cmd.getBufferLength();
        }

        public boolean isSameLayer() {
            return sameLayer;
        }

        public AddressedCommand getCommand() {
            return cmd;
        }
    }

    private final Connection           connection;
    private final Queue<BufferElement> buffer = new ArrayDeque<>();

    private final int bufferSize;
    private       int currentBufferContent = 0;
    private       int currentEcho          = 0x100;

    private Collection<LockCondition> lockConditions  = new ArrayList<>();
    private boolean                   supports32Locks = false;

    private void moveEchoValue() {
        currentEcho++;
        if (currentEcho > 0xffff) {
            currentEcho = 0x100;
        }
    }

    public ConnectionBuffer(Connection connection, int bufferSize) {
        this.connection = connection;
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
        boolean removeUpTo = true;
        for (Iterator<BufferElement> iter = buffer.iterator(); iter.hasNext(); ) {
            BufferElement element = iter.next();
            if (element.isSameLayer() && element.getCommand().getContent().getEchoValue() == sequence.getEchoValue()) {
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
        moveEchoValue();
        buffer.add(element);
        currentBufferContent += element.length;
        connection.send(element.getCommand());
        return true;
    }

    public boolean send(AddressedCommand cmd, boolean ignoreLength) {
        return send(new BufferElement(cmd), ignoreLength);
    }

    public boolean send(CommandSequence seq, boolean ignoreLength) {
        return send(new BufferElement(seq), ignoreLength);
    }

    public boolean send(CommandExecutionPath path, boolean ignoreLength) {
        return send(new BufferElement(path), ignoreLength);
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

}
