package org.zcode.javareceiver.execution;

import java.util.BitSet;
import java.util.NoSuchElementException;
import java.util.Optional;
import java.util.OptionalInt;

import org.zcode.javareceiver.core.Zcode;
import org.zcode.javareceiver.core.ZcodeCommandOutStream;
import org.zcode.javareceiver.core.ZcodeOutStream;
import org.zcode.javareceiver.core.ZcodeStatus;
import org.zcode.javareceiver.semanticParser.ContextView;
import org.zcode.javareceiver.tokenizer.BlockIterator;
import org.zcode.javareceiver.tokenizer.OptIterator;
import org.zcode.javareceiver.tokenizer.Zchars;
import org.zcode.javareceiver.tokenizer.ZcodeTokenBuffer.TokenReader.ReadToken;

public class ZcodeCommandContext {
    private final Zcode          zcode;
    private final ContextView    contextView;
    private final ZcodeOutStream out;

    public ZcodeCommandContext(final Zcode zcode, final ContextView contextView, final ZcodeOutStream out) {
        this.zcode = zcode;
        this.contextView = contextView;
        this.out = out;
    }

    public ZcodeCommandOutStream getOutStream() {
        return out;
    }

    public OptionalInt getField(final byte f) {
        final OptIterator<ReadToken> it = iteratorToMarker();
        for (Optional<ReadToken> opt = it.next(); opt.isPresent(); opt = it.next()) {
            final ReadToken token = opt.get();
            if (token.getKey() == f) {
                return OptionalInt.of(token.getData16());
            }
        }
        return OptionalInt.empty();
    }

    public boolean hasField(final byte f) {
        return getField(f).isPresent();
    }

    public int getField(final byte f, final int def) {
        return getField(f).orElse(def);
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

    public void status(final byte status) {
        contextView.setStatus(status);
        out.writeField('S', status);
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

    public OptIterator<ZcodeField> fieldIterator() {
        return new OptIterator<ZcodeField>() {
            OptIterator<ReadToken> iter = iteratorToMarker();

            @Override
            public Optional<ZcodeField> next() {
                return iter.next().map(r -> new ZcodeField(r));
            }
        };
    }

    public boolean isEmpty() {
        return contextView.getReader().getFirstReadToken().isMarker();
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
                    status(ZcodeStatus.REPEATED_KEY);
                    return false;
                }
                foundCmds.set(key - 'A');
                if (rt.getDataSize() > 2) {
                    commandComplete();
                    status(ZcodeStatus.FIELD_TOO_LONG);
                    return false;
                }
            } else if (!Zchars.isBigField(key)) {
                commandComplete();
                status(ZcodeStatus.INVALID_KEY);
                return false;
            }
        }
        return true;
    }

    public void commandComplete() {
        contextView.setCommandComplete(true);
    }

    public void commandNotComplete() {
        contextView.setCommandComplete(false);
    }

    public boolean isCommandComplete() {
        return contextView.isCommandComplete();
    }

    public boolean isActivated() {
        return contextView.isActivated();
    }

    public void activate() {
        contextView.activate();
    }

    public void silentSucceed() {
        contextView.silentSucceed();
    }

    public Zcode getZcode() {
        return zcode;
    }

    public int getChannelIndex() {
        return contextView.getChannelIndex();
    }
}
