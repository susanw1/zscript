package net.zscript.javareceiver.tokenizer;

import java.util.NoSuchElementException;
import java.util.Optional;
import java.util.OptionalInt;
import java.util.function.Supplier;

import net.zscript.javareceiver.tokenizer.TokenBuffer.TokenReader.ReadToken;
import net.zscript.util.OptIterator;

public class ZscriptTokenExpression implements ZscriptExpression {
    private final Supplier<OptIterator<ReadToken>> tokenIteratorSupplier;

    public ZscriptTokenExpression(final Supplier<OptIterator<ReadToken>> tokenIteratorSupplier) {
        this.tokenIteratorSupplier = tokenIteratorSupplier;
    }

    public OptIterator<ReadToken> iteratorToMarker() {
        return new OptIterator<ReadToken>() {
            final OptIterator<ReadToken> internal = tokenIteratorSupplier.get();

            @Override
            public Optional<ReadToken> next() {
                return internal.next().filter(t -> !t.isMarker());
            }
        };
    }

    @Override
    public OptionalInt getField(byte f) {
        OptIterator<ReadToken> it = iteratorToMarker();
        for (Optional<ReadToken> opt = it.next(); opt.isPresent(); opt = it.next()) {
            ReadToken token = opt.get();
            if (token.getKey() == f) {
                return OptionalInt.of(token.getData16());
            }
        }
        return OptionalInt.empty();
    }

    @Override
    public int getFieldCount() {
        int count = 0;

        OptIterator<ReadToken> it = iteratorToMarker();
        for (Optional<ReadToken> opt = it.next(); opt.isPresent(); opt = it.next()) {
            if (Zchars.isNumericKey(opt.get().getKey())) {
                count++;
            }
        }
        return count;
    }

    @Override
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

    @Override
    public int getBigFieldSize() {
        int size = 0;

        OptIterator<ReadToken> it = iteratorToMarker();
        for (Optional<ReadToken> opt = it.next(); opt.isPresent(); opt = it.next()) {
            ReadToken token = opt.get();
            if (Zchars.isBigField(token.getKey())) {
                size += token.getDataSize();
            }
        }
        return size;
    }
}
