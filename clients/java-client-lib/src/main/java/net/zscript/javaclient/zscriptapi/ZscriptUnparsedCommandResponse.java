package net.zscript.javaclient.zscriptapi;

import java.util.Iterator;
import java.util.NoSuchElementException;
import java.util.Optional;

import net.zscript.javareceiver.tokenizer.BlockIterator;
import net.zscript.javareceiver.tokenizer.TokenBuffer.TokenReader.ReadToken;
import net.zscript.javareceiver.tokenizer.Zchars;
import net.zscript.util.OptIterator;

public class ZscriptUnparsedCommandResponse {
    private final ReadToken first;
    // TODO: split common code from CommandContext

    public ZscriptUnparsedCommandResponse(ReadToken first) {
        this.first = first;
    }

    private OptIterator<ReadToken> iterator() {
        return new OptIterator<ReadToken>() {
            OptIterator<ReadToken> internal = first.getNextTokens();

            @Override
            public Optional<ReadToken> next() {
                return internal.next().filter(t -> !t.isMarker());
            }

        };
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

    public String getBigFieldString() {
        StringBuilder str = new StringBuilder();
        for (Iterator<Byte> iterator = getBigField(); iterator.hasNext();) {
            str.append((char) (byte) iterator.next());
        }
        return str.toString();
    }

    public byte[] getBigFieldData() {
        byte[] data = new byte[getBigFieldSize()];
        int    i    = 0;
        for (Iterator<Byte> iterator = getBigField(); iterator.hasNext();) {
            data[i] = iterator.next();
        }
        return data;
    }

    public Optional<Integer> getField(char c) {
        return getField((byte) c);
    }

}
