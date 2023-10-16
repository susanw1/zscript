package net.zscript.javaclient.connection;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.UncheckedIOException;
import java.util.ArrayDeque;
import java.util.Deque;
import java.util.Iterator;
import java.util.Queue;
import java.util.function.Consumer;

import net.zscript.javaclient.commandbuilder.commandnodes.CommandSequenceNode;
import net.zscript.javaclient.commandbuilder.Utils;
import net.zscript.javaclient.commandbuilder.ZscriptByteString;
import net.zscript.javaclient.connection.ResponseParser.ResponseHeader;
import net.zscript.model.components.Zchars;

/**
 * Represents a Device view of a connection.
 */
public class CommandResponseQueue implements DeviceNode {
    private static final int MAX_SENT = 10;

    private final ZscriptConnection connection;
    private final RemoteConnectors  addrSystem = new RemoteConnectors(this);

    private final Deque<CommandEntry> sent   = new ArrayDeque<>();
    private final Queue<CommandEntry> toSend = new ArrayDeque<>();

    private int     currentAutoEchoNumber = 0;
    private boolean canPipeline           = true;

    public CommandResponseQueue(ZscriptConnection connection) {
        this.connection = connection;
        connection.onReceive(responseBytes -> {
            ResponseHeader header = ResponseParser.parseResponseHeader(responseBytes);
            if (header.hasAddress()) {
                addrSystem.response(header.getAddress(), responseBytes);
            } else {
                callback(responseBytes, header.getEcho(), header.getType());
            }
        });
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void send(final ZscriptAddress addr, final byte[] zscriptCommandSequence) throws IOException {
        if (sent.size() < MAX_SENT && canPipeline) {
            AddrCommandSeqElEntry el = new AddrCommandSeqElEntry(addr, zscriptCommandSequence);
            sent.add(el);
            connection.send(el.compile());
        } else {
            toSend.add(new AddrCommandSeqElEntry(addr, zscriptCommandSequence));
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void send(final CommandSequenceNode sequence) throws IOException {
        CommandSeqElEntry el = new CommandSeqElEntry(sequence, currentAutoEchoNumber);
        if (sent.size() < MAX_SENT && canPipeline) {
            sent.add(el);
            connection.send(el.compile());
        } else {
            toSend.add(el);
        }

        currentAutoEchoNumber++;
        if (currentAutoEchoNumber >= 0x10000) {
            currentAutoEchoNumber = 0;
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void send(final byte[] zscriptCommandSequence, final Consumer<byte[]> callback) throws IOException {
        ByteArrEntry e = new ByteArrEntry(callback, zscriptCommandSequence);
        if (sent.isEmpty()) {
            sent.add(e);
            connection.send(e.compile());
            canPipeline = false;
        } else {
            toSend.add(e);
        }
    }

    /**
     * @param response
     * @param echo
     * @param respType
     */
    private void callback(final byte[] response, int echo, int respType) {
        if (respType != 0) {
            // TODO: notifications
        } else if (!canPipeline) {
            ((ByteArrEntry) sent.poll()).callback(response);
            canPipeline = true;
        } else {
            boolean found = false;
            for (final Iterator<CommandEntry> iterator = sent.iterator(); iterator.hasNext(); ) {
                CommandEntry entryPlain = iterator.next();
                if (entryPlain.getClass() == CommandSeqElEntry.class && ((CommandSeqElEntry) entryPlain).getEcho() == echo) {
                    ((CommandSeqElEntry) entryPlain).callback(response);
                    iterator.remove();
                    found = true;
                    break;
                }
            }
            if (!found) {
                throw new IllegalStateException("Command Response system doesn't know this response");
            }
        }
        if (!toSend.isEmpty()) {
            CommandEntry entry = toSend.peek();
            if (sent.isEmpty()) {
                sent.add(entry);
                canPipeline = entry.canBePipelined();
                toSend.poll();
            } else if (sent.size() < MAX_SENT && canPipeline && entry.canBePipelined()) {
                sent.add(toSend.poll());
            }
        }
    }

    @Override
    public RemoteConnectors getRemoteConnectors() {
        return new RemoteConnectors(this);
    }

    private interface CommandEntry {
        byte[] compile();

        boolean canBePipelined();
    }

    private class CommandSeqElEntry implements CommandEntry {
        private final CommandSequenceNode cmdSeq;
        private final int                 echo;

        public CommandSeqElEntry(final CommandSequenceNode cmdSeq, final int echo) {
            this.cmdSeq = cmdSeq;
            this.echo = echo;
        }

        @Override
        public byte[] compile() {
            // TODO: decide on how locking will work...
            return ZscriptByteString.builder()
                    .appendField(Zchars.Z_ECHO, echo)
                    .appendRaw(cmdSeq.compile())
                    .appendByte(Zchars.Z_NEWLINE)
                    .toByteArray();
        }

        public void callback(final byte[] received) {
            ResponseParser.parseFullResponse(cmdSeq, received);
        }

        public int getEcho() {
            return echo;
        }

        @Override
        public boolean canBePipelined() {
            return true;
        }
    }

    private class AddrCommandSeqElEntry implements CommandEntry {
        private final ZscriptAddress addr;
        private final byte[]         cmdSeq;

        public AddrCommandSeqElEntry(final ZscriptAddress addr, final byte[] cmdSeq) {
            this.addr = addr;
            this.cmdSeq = cmdSeq;
        }

        @Override
        public byte[] compile() {
            // TODO: decide on how locking will work...
            ByteArrayOutputStream str = new ByteArrayOutputStream(addr.size() * 3 + cmdSeq.length);
            try {
                // FIXME: use addr's own outputter here
                boolean isFirst = true;
                for (int i : addr.getAsInts()) {
                    str.write(Utils.formatField((byte) (isFirst ? '@' : '.'), i));
                    isFirst = false;
                }
                str.write(cmdSeq);
            } catch (IOException e) {
                throw new UncheckedIOException(e);
            }
            return str.toByteArray();
        }

        @Override
        public boolean canBePipelined() {
            return true;
        }
    }

    private class ByteArrEntry implements CommandEntry {
        private final Consumer<byte[]> callback;
        private final byte[]           data;

        public ByteArrEntry(final Consumer<byte[]> callback, final byte[] data) {
            this.callback = callback;
            this.data = data;
        }

        @Override
        public byte[] compile() {
            return data;
        }

        public void callback(final byte[] received) {
            callback.accept(received);
        }

        @Override
        public boolean canBePipelined() {
            return false;
        }
    }
}
