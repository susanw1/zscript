package org.zcode.javareceiver.execution;

import java.util.NoSuchElementException;
import java.util.Optional;

import org.zcode.javareceiver.core.ZcodeOutStream;
import org.zcode.javareceiver.core.ZcodeStatus;
import org.zcode.javareceiver.tokenizer.BlockIterator;
import org.zcode.javareceiver.tokenizer.OptIterator;
import org.zcode.javareceiver.tokenizer.Zchars;
import org.zcode.javareceiver.tokenizer.ZcodeTokenBuffer.TokenReader.ReadToken;

public class ZcodeCommandEnviroment {
    private final ZcodeCommandView view;

    private boolean statusGiven = false;

    public ZcodeCommandEnviroment(ZcodeCommandView view) {
        this.view = view;
    }

    public ZcodeOutStream getOutStream() {
        return view.getOutStream();
    }

    public OptIterator<ReadToken> tokenIterator() {
        return view.iterator();
    }

    public boolean hasField(byte f) {
        OptIterator<ReadToken> it = view.iterator();
        for (Optional<ReadToken> opt = it.next(); opt.isPresent(); opt = it.next()) {
            ReadToken token = opt.get();
            if (token.getKey() == f) {
                return true;
            }
        }
        return false;
    }

    public Optional<Integer> getField(byte f) {
        OptIterator<ReadToken> it = view.iterator();
        for (Optional<ReadToken> opt = it.next(); opt.isPresent(); opt = it.next()) {
            ReadToken token = opt.get();
            if (token.getKey() == f) {
                return Optional.of(token.getData16());
            }
        }
        return Optional.empty();
    }

    public int getField(byte f, int def) {
        OptIterator<ReadToken> it = view.iterator();
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

        OptIterator<ReadToken> it = view.iterator();
        for (Optional<ReadToken> opt = it.next(); opt.isPresent(); opt = it.next()) {
            ReadToken token = opt.get();
            if ('A' <= token.getKey() && token.getKey() <= 'Z') {
                count++;
            }
        }
        return count;
    }

    public BlockIterator getBigField() {
        return new BlockIterator() {
            OptIterator<ReadToken> it = view.iterator();

            BlockIterator internal = null;

            @Override
            public boolean hasNext() {
                if (internal != null && internal.hasNext()) {
                    return true;
                }
                for (Optional<ReadToken> opt = it.next(); opt.isPresent(); opt = it.next()) {
                    ReadToken token = opt.get();
                    if (token.getKey() == Zchars.Z_BIGFIELD_HEX || token.getKey() == Zchars.Z_BIGFIELD_QUOTED) {
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

        OptIterator<ReadToken> it = view.iterator();
        for (Optional<ReadToken> opt = it.next(); opt.isPresent(); opt = it.next()) {
            ReadToken token = opt.get();
            if (token.getKey() == Zchars.Z_BIGFIELD_HEX || token.getKey() == Zchars.Z_BIGFIELD_QUOTED) {
                size += token.getDataSize();
            }
        }
        return size;
    }

    public void status(byte status) {
        statusGiven = true;
        if (status != ZcodeStatus.SUCCESS) {
            view.fail(status);
        } else {
            view.getOutStream().writeField('S', ZcodeStatus.SUCCESS);
        }
    }

    public boolean statusGiven() {
        return statusGiven;
    }

    public void setComplete() {
        view.setComplete();
    }

    public void clearComplete() {
        view.clearComplete();
    }

    public boolean isActivated() {
        return view.isActivated();
    }

    public void activate() {
        view.activate();
    }
}
