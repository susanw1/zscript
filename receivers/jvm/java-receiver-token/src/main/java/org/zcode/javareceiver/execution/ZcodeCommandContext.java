package org.zcode.javareceiver.execution;

import java.util.BitSet;
import java.util.NoSuchElementException;
import java.util.Optional;

import org.zcode.javareceiver.core.ZcodeCommandOutStream;
import org.zcode.javareceiver.core.ZcodeOutStream;
import org.zcode.javareceiver.core.ZcodeStatus;
import org.zcode.javareceiver.semanticParser.ParseState;
import org.zcode.javareceiver.tokenizer.BlockIterator;
import org.zcode.javareceiver.tokenizer.OptIterator;
import org.zcode.javareceiver.tokenizer.Zchars;
import org.zcode.javareceiver.tokenizer.ZcodeTokenBuffer.TokenReader.ReadToken;

public class ZcodeCommandContext {
    private final ParseState     parser;
    private final ZcodeOutStream out;

    private boolean statusGiven = false;

    public ZcodeCommandContext(final ParseState parser, final ZcodeOutStream out) {
        this.parser = parser;
        this.out = out;
    }

    public ZcodeCommandOutStream getOutStream() {
        return out;
    }

    public boolean hasField(final byte f) {
        final OptIterator<ReadToken> it = iterator();
        for (Optional<ReadToken> opt = it.next(); opt.isPresent(); opt = it.next()) {
            final ReadToken token = opt.get();
            if (token.getKey() == f) {
                return true;
            }
        }
        return false;
    }

    public Optional<Integer> getField(final byte f) {
        final OptIterator<ReadToken> it = iterator();
        for (Optional<ReadToken> opt = it.next(); opt.isPresent(); opt = it.next()) {
            final ReadToken token = opt.get();
            if (token.getKey() == f) {
                return Optional.of(token.getData16());
            }
        }
        return Optional.empty();
    }

    public int getField(final byte f, final int def) {
        final OptIterator<ReadToken> it = iterator();
        for (Optional<ReadToken> opt = it.next(); opt.isPresent(); opt = it.next()) {
            final ReadToken token = opt.get();
            if (token.getKey() == f) {
                return token.getData16();
            }
        }
        return def;
    }

    public int getFieldCount() {
        int count = 0;

        final OptIterator<ReadToken> it = iterator();
        for (Optional<ReadToken> opt = it.next(); opt.isPresent(); opt = it.next()) {
            if (Zchars.isNumericKey(opt.get().getKey())) {
                count++;
            }
        }
        return count;
    }

    public BlockIterator getBigField() {
        return new BlockIterator() {
            OptIterator<ReadToken> it = iterator();

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

        final OptIterator<ReadToken> it = iterator();
        for (Optional<ReadToken> opt = it.next(); opt.isPresent(); opt = it.next()) {
            final ReadToken token = opt.get();
            if (Zchars.isBigField(token.getKey())) {
                size += token.getDataSize();
            }
        }
        return size;
    }

    public void status(final byte status) {
        statusGiven = true;
        if (!ZcodeStatus.isSuccess(status)) {
            if (ZcodeStatus.isSystemError(status)) {
                parser.error();
            } else {
                parser.softFail();
            }
        }
        out.writeField('S', status);
    }

    public boolean statusGiven() {
        return statusGiven;
    }

    private OptIterator<ReadToken> iterator() {
        return new OptIterator<ReadToken>() {
            OptIterator<ReadToken> internal = parser.getReader().iterator();

            @Override
            public Optional<ReadToken> next() {
                return internal.next().filter(t -> !t.isMarker());
            }

        };
    }

    public OptIterator<ZcodeField> fieldIterator() {
        return new OptIterator<ZcodeField>() {
            OptIterator<ReadToken> iter = iterator();

            @Override
            public Optional<ZcodeField> next() {
                return iter.next().map(r -> new ZcodeField(r));
            }
        };
    }

    public boolean isEmpty() {
        return parser.getReader().getFirstReadToken().isMarker();
    }

    public boolean verify() {
        final OptIterator<ReadToken> iter      = iterator();
        final BitSet                 foundCmds = new BitSet(26);
        for (Optional<ReadToken> opt = iter.next(); opt.isPresent(); opt = iter.next()) {
            final ReadToken rt  = opt.get();
            final byte      key = rt.getKey();
            if (Zchars.isNumericKey(key)) {
                if (foundCmds.get(key - 'A')) {
                    setComplete();
                    error(ZcodeStatus.TOKEN_ERROR);
                    return false;
                }
                foundCmds.set(key - 'A');
                if (rt.getDataSize() > 2) {
                    setComplete();
                    error(ZcodeStatus.TOKEN_ERROR);
                    return false;
                }
            } else if (key != '"' && key != '+') {
                setComplete();
                error(ZcodeStatus.TOKEN_ERROR);
                return false;
            }
        }
        return true;
    }

    private void error(final byte status) {
        if (!out.isOpen()) {
            out.open();
        }
        out.writeField('S', status);
        out.endSequence();
        out.close();
        parser.error();
    }

    public void setComplete() {
        parser.setComplete(true);
    }

    public void clearComplete() {
        parser.setComplete(false);
    }

    public boolean isComplete() {
        return parser.isComplete();
    }

    public boolean isActivated() {
        return parser.isActivated();
    }

    public void activate() {
        parser.activate();
    }

}
