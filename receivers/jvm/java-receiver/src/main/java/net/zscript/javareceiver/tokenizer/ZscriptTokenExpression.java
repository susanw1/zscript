package net.zscript.javareceiver.tokenizer;

import java.util.NoSuchElementException;
import java.util.Optional;
import java.util.OptionalInt;
import java.util.function.Supplier;

import net.zscript.javareceiver.tokenizer.TokenBuffer.TokenReader.ReadToken;
import net.zscript.model.components.Zchars;
import net.zscript.util.ByteString;
import net.zscript.util.OptIterator;

/**
 *
 */
public class ZscriptTokenExpression implements ZscriptExpression {
    private final Supplier<OptIterator<ReadToken>> tokenIteratorSupplier;

    /**
     * Creates a {@link ZscriptTokenExpression} using the specified Supplier to acquire the {@link OptIterator} for reading the {@link ReadToken}s.
     *
     * @param tokenIteratorSupplier where to get the ReadToken iterator
     */
    public ZscriptTokenExpression(final Supplier<OptIterator<ReadToken>> tokenIteratorSupplier) {
        this.tokenIteratorSupplier = tokenIteratorSupplier;
    }

    /**
     * Creates a special ReadToken iterator which stops at the first Marker (and hence reads exactly one ZscriptExpression).
     *
     * @return a ReadToken iterator
     */
    public OptIterator<ReadToken> iteratorToMarker() {
        return new OptIterator<>() {
            final OptIterator<ReadToken> internal = tokenIteratorSupplier.get();

            @Override
            public Optional<ReadToken> next() {
                return internal.next().filter(t -> !t.isMarker());
            }
        };
    }

    @Override
    public OptionalInt getField(byte key) {
        return iteratorToMarker().stream()
                .filter(tok -> tok.getKey() == key)
                .mapToInt(ReadToken::getData16)
                .findFirst();
    }

    @Override
    public int getFieldCount() {
        return (int) iteratorToMarker().stream()
                .filter(tok -> Zchars.isNumericKey(tok.getKey()))
                .count();
    }

    @Override
    public boolean hasBigField() {
        return iteratorToMarker().stream()
                .anyMatch(tok -> Zchars.isBigField(tok.getKey()));
    }

    @Override
    public int getBigFieldSize() {
        return iteratorToMarker().stream()
                .filter(tok -> Zchars.isBigField(tok.getKey()))
                .mapToInt(ReadToken::getDataSize)
                .sum();
    }
    
    @Override
    public ByteString getBigFieldAsByteString() {
        ByteString.ByteStringBuilder b = ByteString.builder();

        for (BlockIterator iterator = getBigField(); iterator.hasNext(); ) {
            b.appendRaw(iterator.nextContiguous());
        }
        return b.build();
    }

    public BlockIterator getBigField() {
        return new BlockIterator() {
            final OptIterator<ReadToken> it = iteratorToMarker();

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

}
