package net.zscript.javaclient.connection;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.util.ArrayDeque;
import java.util.Deque;
import java.util.Iterator;
import java.util.Queue;
import java.util.function.Consumer;

import net.zscript.javaclient.commandbuilder.CommandSeqElement;
import net.zscript.javaclient.commandbuilder.ZscriptCommandBuilder;
import net.zscript.javaclient.connection.ResponseParser.ResponseHeader;

public class CommandResponseQueue implements CommandResponseSystem {
    private static final int MAX_SENT = 10;

    private final ResponseAddressingSystem addrSystem = new ResponseAddressingSystem(this);

    private final ZscriptConnection connection;

    private interface CommandEntry {
        byte[] compile();

        boolean canBePipelined();
    }

    private class CommandSeqElEntry implements CommandEntry {
        private final CommandSeqElement cmdSeq;
        private final int               echo;

        public CommandSeqElEntry(final CommandSeqElement cmdSeq, final int echo) {
            this.cmdSeq = cmdSeq;
            this.echo = echo;
        }

        @Override
        public byte[] compile() {
            // TODO: decide on how locking will work...
            byte[] echoF     = ZscriptCommandBuilder.writeField((byte) '_', echo);
            byte[] startData = cmdSeq.compile(false);

            ByteArrayOutputStream str = new ByteArrayOutputStream(startData.length + echoF.length + 1);
            try {
                str.write(echoF);
                str.write(startData);
                str.write('\n');
            } catch (IOException e) {
                throw new RuntimeException(e);
            }
            return str.toByteArray();
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
        private final byte[]         cmdSeq;
        private final ZscriptAddress addr;

        public AddrCommandSeqElEntry(final byte[] cmdSeq, final ZscriptAddress addr) {
            this.cmdSeq = cmdSeq;
            this.addr = addr;
        }

        @Override
        public byte[] compile() {
            // TODO: decide on how locking will work...
            ByteArrayOutputStream str = new ByteArrayOutputStream(addr.getAddr().length * 3 + cmdSeq.length);
            try {
                boolean isFirst = true;
                for (int i : addr.getAddr()) {
                    str.write(ZscriptCommandBuilder.writeField((byte) (isFirst ? '@' : '.'), i));
                    isFirst = false;
                }
                str.write(cmdSeq);
            } catch (IOException e) {
                throw new RuntimeException(e);
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

    private final Deque<CommandEntry> sent   = new ArrayDeque<>();
    private final Queue<CommandEntry> toSend = new ArrayDeque<>();

    private int     currentEcho = 0;
    private boolean canPipeline = true;

    public CommandResponseQueue(ZscriptConnection connection) {
        this.connection = connection;
        connection.onReceive(resp -> {
            ResponseHeader header = ResponseParser.parseResponseHeader(resp);
            if (header.getAddr().length == 0) {
                callback(resp, header.getEcho(), header.getType());
            } else {
                addrSystem.response(header.getAddr(), resp);
            }
        });
    }

    @Override
    public void send(final ZscriptAddress addr, final byte[] data) {
        if (sent.size() < MAX_SENT && canPipeline) {
            AddrCommandSeqElEntry el = new AddrCommandSeqElEntry(data, addr);
            sent.add(el);
            connection.send(el.compile());
        } else {
            toSend.add(new AddrCommandSeqElEntry(data, addr));
        }
    }

    @Override
    public void send(final CommandSeqElement seq) {
        if (sent.size() < MAX_SENT && canPipeline) {
            CommandSeqElEntry el = new CommandSeqElEntry(seq, currentEcho);
            sent.add(el);
            connection.send(el.compile());
        } else {
            toSend.add(new CommandSeqElEntry(seq, currentEcho));
        }
        currentEcho++;
        if (currentEcho >= 0x10000) {
            currentEcho = 0;
        }
    }

    @Override
    public void send(final byte[] zscript, final Consumer<byte[]> callback) {
        if (sent.size() == 0) {
            ByteArrEntry el = new ByteArrEntry(callback, zscript);
            sent.add(el);
            connection.send(el.compile());
            canPipeline = false;
        } else {
            toSend.add(new ByteArrEntry(callback, zscript));
        }
    }

    private void callback(final byte[] response, int echo, int respType) {
        if (respType != 0) {
            // TODO: notifications
        } else if (!canPipeline) {
            ((ByteArrEntry) sent.poll()).callback(response);
            canPipeline = true;
        } else {
            boolean found = false;
            for (Iterator<CommandEntry> iterator = sent.iterator(); iterator.hasNext();) {
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
            if (sent.size() == 0) {
                sent.add(entry);
                canPipeline = entry.canBePipelined();
                toSend.poll();
            } else if (sent.size() < MAX_SENT && canPipeline && entry.canBePipelined()) {
                sent.add(toSend.poll());
            }
        }
    }

    @Override
    public ResponseAddressingSystem getResponseAddressingSystem() {
        return new ResponseAddressingSystem(this);
    }

}