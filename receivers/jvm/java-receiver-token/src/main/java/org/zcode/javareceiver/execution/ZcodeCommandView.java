package org.zcode.javareceiver.execution;

import java.util.BitSet;
import java.util.NoSuchElementException;
import java.util.Optional;

import org.zcode.javareceiver.core.ZcodeOutStream;
import org.zcode.javareceiver.core.ZcodeStatus;
import org.zcode.javareceiver.semanticParser.SemanticParser;
import org.zcode.javareceiver.tokenizer.BlockIterator;
import org.zcode.javareceiver.tokenizer.OptIterator;
import org.zcode.javareceiver.tokenizer.Zchars;
import org.zcode.javareceiver.tokenizer.ZcodeTokenBuffer.TokenReader.ReadToken;

public class ZcodeCommandView {
    private final SemanticParser parser;
    private final ZcodeOutStream out;

    private boolean statusGiven = false;

    public ZcodeCommandView(SemanticParser parser, ZcodeOutStream out) {
        this.parser = parser;
        this.out = out;
    }

    public ZcodeOutStream getOutStream() {
        return out;
    }

    public boolean hasField(byte f) {
        OptIterator<ReadToken> it = iterator();
        for (Optional<ReadToken> opt = it.next(); opt.isPresent(); opt = it.next()) {
            ReadToken token = opt.get();
            if (token.getKey() == f) {
                return true;
            }
        }
        return false;
    }

    public Optional<Integer> getField(byte f) {
        OptIterator<ReadToken> it = iterator();
        for (Optional<ReadToken> opt = it.next(); opt.isPresent(); opt = it.next()) {
            ReadToken token = opt.get();
            if (token.getKey() == f) {
                return Optional.of(token.getData16());
            }
        }
        return Optional.empty();
    }

    public int getField(byte f, int def) {
        OptIterator<ReadToken> it = iterator();
        for (Optional<ReadToken> opt = it.next(); opt.isPresent(); opt = it.next()) {
            ReadToken token = opt.get();
            if (token.getKey() == f) {
                return token.getData16();
            }
        }
        return def;
    }

    public int getFieldCount() {
        int count = 0;

        OptIterator<ReadToken> it = iterator();
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
                    ReadToken token = opt.get();
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
            public byte[] nextContiguous(int maxLength) {
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

        OptIterator<ReadToken> it = iterator();
        for (Optional<ReadToken> opt = it.next(); opt.isPresent(); opt = it.next()) {
            ReadToken token = opt.get();
            if (Zchars.isBigField(token.getKey())) {
                size += token.getDataSize();
            }
        }
        return size;
    }

    public void status(byte status) {
        statusGiven = true;
        if (status != ZcodeStatus.SUCCESS) {
            if (status < 0x10) {
                parser.error();
            } else {
                parser.softFail();
            }
        }
        out.writeField('S', ZcodeStatus.SUCCESS);
    }

    public boolean statusGiven() {
        return statusGiven;
    }

    public OptIterator<ReadToken> iterator() {
        return new OptIterator<ReadToken>() {
            OptIterator<ReadToken> internal = parser.getReader().iterator();

            @Override
            public Optional<ReadToken> next() {
                return internal.next().filter(t -> !t.isMarker());
            }

        };
    }

    public boolean verify() {
        OptIterator<ReadToken> iter      = iterator();
        BitSet                 foundCmds = new BitSet(26);
        for (Optional<ReadToken> opt = iter.next(); opt.isPresent(); opt = iter.next()) {
            ReadToken rt  = opt.get();
            byte      key = rt.getKey();
            if ('A' <= key && key <= 'Z') {
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

    private void error(byte status) {
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

    public boolean isActivated() {
        return parser.isActivated();
    }

    public boolean isEmpty() {
        return parser.getReader().getFirstReadToken().isMarker();
    }

    public void activate() {
        parser.activate();
    }

    public boolean isComplete() {
        return parser.isComplete();
    }

}
