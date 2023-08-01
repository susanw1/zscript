package net.zscript.javareceiver.execution;

import java.util.BitSet;
import java.util.Iterator;
import java.util.NoSuchElementException;
import java.util.Optional;
import java.util.OptionalInt;

import net.zscript.javareceiver.core.Zscript;
import net.zscript.javareceiver.core.ZscriptCommandOutStream;
import net.zscript.javareceiver.core.OutStream;
import net.zscript.javareceiver.core.ZscriptStatus;
import net.zscript.javareceiver.semanticParser.ContextView;
import net.zscript.javareceiver.tokenizer.BlockIterator;
import net.zscript.javareceiver.tokenizer.OptIterator;
import net.zscript.javareceiver.tokenizer.Zchars;
import net.zscript.javareceiver.tokenizer.TokenBuffer.TokenReader.ReadToken;

public class CommandContext extends AbstractContext {
    private final Zscript          zscript;
    private final OutStream out;

    public CommandContext(final Zscript zscript, final ContextView contextView, final OutStream out) {
        super(contextView);
        this.zscript = zscript;
        this.out = out;
    }

    public ZscriptCommandOutStream getOutStream() {
        return out.asCommandOutStream();
    }

    public OptionalInt getField(final byte key) {
        final OptIterator<ReadToken> it = iteratorToMarker();
        for (Optional<ReadToken> opt = it.next(); opt.isPresent(); opt = it.next()) {
            final ReadToken token = opt.get();
            if (token.getKey() == key) {
                return OptionalInt.of(token.getData16());
            }
        }
        return OptionalInt.empty();
    }

    public boolean hasField(final byte key) {
        return getField(key).isPresent();
    }

    public int getField(final byte key, final int def) {
        return getField(key).orElse(def);
    }

    public int getFieldCount() {
        int count = 0;

        final OptIterator<ReadToken> it = iteratorToMarker();
        for (Optional<ReadToken> opt = it.next(); opt.isPresent(); opt = it.next()) {
            if (Zchars.isNumericKey(opt.get().getKey())) {
                count++;
            }
        }
        return count;
    }

    public BlockIterator getBigField() {
        return new BlockIterator() {
            OptIterator<ReadToken> it = iteratorToMarker();

            BlockIterator internal = null;

            @Override
            public boolean hasNext() {
                if (internal != null && internal.hasNext()) {
                    return true;
                }
                for (Optional<ReadToken> opt = it.next(); opt.isPresent(); opt = it.next()) {
                    final ReadToken token = opt.get();
                    if (Zchars.isBigField(token.getKey())) {
                        internal = token.blockIterator();
                        if (internal.hasNext()) {
                            return true;
                        }
                    }
                }
                return false;
            }

            @Override
            public Byte next() {
                if (!hasNext()) {
                    throw new NoSuchElementException();
                }
                return internal.next();
            }

            @Override
            public byte[] nextContiguous(final int maxLength) {
                if (!hasNext()) {
                    throw new NoSuchElementException();
                }
                return internal.nextContiguous(maxLength);
            }

            @Override
            public byte[] nextContiguous() {
                if (!hasNext()) {
                    throw new NoSuchElementException();
                }
                return internal.nextContiguous();
            }
        };
    }

    public int getBigFieldSize() {
        int size = 0;

        final OptIterator<ReadToken> it = iteratorToMarker();
        for (Optional<ReadToken> opt = it.next(); opt.isPresent(); opt = it.next()) {
            final ReadToken token = opt.get();
            if (Zchars.isBigField(token.getKey())) {
                size += token.getDataSize();
            }
        }
        return size;
    }

    public byte[] getBigFieldData() {
        byte[] data = new byte[getBigFieldSize()];
        int    i    = 0;
        for (Iterator<Byte> iterator = getBigField(); iterator.hasNext();) {
            data[i++] = iterator.next();
        }
        return data;
    }

    public OptionalInt getField(char c) {
        return getField((byte) c);
    }

    @Override
    public boolean status(final byte status) {
        if (super.status(status)) {
            out.asCommandOutStream().writeField(Zchars.Z_STATUS, status);
            return true;
        }
        return false;
    }

    private OptIterator<ReadToken> iteratorToMarker() {
        return new OptIterator<ReadToken>() {
            OptIterator<ReadToken> internal = contextView.getReader().iterator();

            @Override
            public Optional<ReadToken> next() {
                return internal.next().filter(t -> !t.isMarker());
            }

        };
    }

    public OptIterator<ZscriptField> fieldIterator() {
        return new OptIterator<ZscriptField>() {
            OptIterator<ReadToken> iter = iteratorToMarker();

            @Override
            public Optional<ZscriptField> next() {
                return iter.next().map(r -> new ZscriptField(r));
            }
        };
    }

    public boolean verify() {
        final OptIterator<ReadToken> iter      = iteratorToMarker();
        final BitSet                 foundCmds = new BitSet(26);
        for (Optional<ReadToken> opt = iter.next(); opt.isPresent(); opt = iter.next()) {
            final ReadToken rt  = opt.get();
            final byte      key = rt.getKey();
            if (Zchars.isNumericKey(key)) {
                if (foundCmds.get(key - 'A')) {
                    commandComplete();
                    status(ZscriptStatus.REPEATED_KEY);
                    return false;
                }
                foundCmds.set(key - 'A');
                if (rt.getDataSize() > 2) {
                    commandComplete();
                    status(ZscriptStatus.FIELD_TOO_LONG);
                    return false;
                }
            } else if (!Zchars.isBigField(key)) {
                commandComplete();
                status(ZscriptStatus.INVALID_KEY);
                return false;
            }
        }
        return true;
    }

    public void activate() {
        contextView.activate();
    }

    public Zscript getZcode() {
        return zscript;
    }

    public int getChannelIndex() {
        return contextView.getChannelIndex();
    }
}
