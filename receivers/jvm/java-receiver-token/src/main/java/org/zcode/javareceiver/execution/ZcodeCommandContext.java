package org.zcode.javareceiver.execution;

import java.util.BitSet;
import java.util.NoSuchElementException;
import java.util.Optional;

import org.zcode.javareceiver.core.ZcodeCommandOutStream;
import org.zcode.javareceiver.core.ZcodeOutStream;
import org.zcode.javareceiver.core.ZcodeStatus;
import org.zcode.javareceiver.semanticParser.ContextView;
import org.zcode.javareceiver.tokenizer.BlockIterator;
import org.zcode.javareceiver.tokenizer.OptIterator;
import org.zcode.javareceiver.tokenizer.Zchars;
import org.zcode.javareceiver.tokenizer.ZcodeTokenBuffer.TokenReader.ReadToken;

public class ZcodeCommandContext {
    private final ContextView    contextView;
    private final ZcodeOutStream out;

//    private final boolean statusGiven = false;
//    private final boolean statusError = false;

    public ZcodeCommandContext(final ContextView contextView, final ZcodeOutStream out) {
        this.contextView = contextView;
        this.out = out;
    }

    public ZcodeCommandOutStream getOutStream() {
        return out;
    }

    public boolean hasField(final byte f) {
        final OptIterator<ReadToken> it = iteratorToMarker();
        for (Optional<ReadToken> opt = it.next(); opt.isPresent(); opt = it.next()) {
            final ReadToken token = opt.get();
            if (token.getKey() == f) {
                return true;
            }
        }
        return false;
    }

    public Optional<Integer> getField(final byte f) {
        final OptIterator<ReadToken> it = iteratorToMarker();
        for (Optional<ReadToken> opt = it.next(); opt.isPresent(); opt = it.next()) {
            final ReadToken token = opt.get();
            if (token.getKey() == f) {
                return Optional.of(token.getData16());
            }
        }
        return Optional.empty();
    }

    public int getField(final byte f, final int def) {
        final OptIterator<ReadToken> it = iteratorToMarker();
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
    }

//    private void error(final byte status) {
//        if (!out.isOpen()) {
//            out.open();
//        }
//        out.writeField('S', status);
//        out.endSequence();
//        out.close();
//        contextView.error();
//    }

//  public void status(final byte status) {
//        statusGiven = true;
//        if (!ZcodeStatus.isSuccess(status)) {
//            if (ZcodeStatus.isError(status)) {
//                statusError = true;
//                contextView.error();
//            } else {
//                contextView.softFail();
//            }
//        }
//        out.writeField('S', status);
//    }
//
//    public boolean statusGiven() {
//        return statusGiven;
//    }
//
//    public boolean statusError() {
//        return statusError;
//    }

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
                    setCommandCanComplete();
                    status(ZcodeStatus.REPEATED_KEY);
                    return false;
                }
                foundCmds.set(key - 'A');
                if (rt.getDataSize() > 2) {
                    setCommandCanComplete();
                    status(ZcodeStatus.FIELD_TOO_LONG);
                    return false;
                }
            } else if (key != '"' && key != '+') {
                setCommandCanComplete();
                status(ZcodeStatus.INVALID_KEY);
                return false;
            }
        }
        return true;
    }

    public void setCommandCanComplete() {
        contextView.setCommandCanComplete(true);
    }

    public void clearCommandCanComplete() {
        contextView.setCommandCanComplete(false);
    }

    public boolean commandCanComplete() {
        return contextView.commandCanComplete();
    }

    public boolean isActivated() {
        return contextView.isActivated();
    }

    public void activate() {
        contextView.activate();
    }

}
