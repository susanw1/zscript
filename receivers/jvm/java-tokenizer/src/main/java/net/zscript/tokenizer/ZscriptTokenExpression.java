package net.zscript.tokenizer;

import javax.annotation.Nonnull;
import java.util.NoSuchElementException;
import java.util.Optional;
import java.util.function.Supplier;
import java.util.stream.Stream;

import static java.util.stream.Collectors.joining;
import static net.zscript.util.ByteString.byteString;

import net.zscript.model.components.Zchars;
import net.zscript.tokenizer.TokenBuffer.TokenReader.ReadToken;
import net.zscript.util.BlockIterator;
import net.zscript.util.ByteString;
import net.zscript.util.OptIterator;

/**
 * Implementation of ZscriptExpression which understands how to locate and extract fields from Tokens.
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
     * Creates a special ReadToken iterator which stops at the first Marker (and hence reads exactly one ZscriptExpression). Note: relies on the tokenIteratorSupplier to supply
     * tokens, which should skip extension tokens correctly.
     *
     * @return a ReadToken iterator
     */
    public OptIterator<ReadToken> iteratorToMarker() {
        return new OptIterator<>() {
            final OptIterator<ReadToken> internal = tokenIteratorSupplier.get();

            @Nonnull
            @Override
            public Optional<ReadToken> next() {
                return internal.next().filter(t -> !t.isMarker());
            }
        };
    }

    @Nonnull
    @Override
    public Stream<ZscriptField> fields() {
        return iteratorToMarker().stream()
                .filter(tok -> Zchars.isNumericKey(tok.getKey()))
                .map(ZscriptTokenField::new);
    }

    @Nonnull
    @Override
    public Optional<? extends ZscriptField> getZscriptField(byte key) {
        return iteratorToMarker().stream()
                .filter(tok -> tok.getKey() == key)
                .findFirst()
                .map(ZscriptTokenField::new);
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

    @Nonnull
    @Override
    public ByteString getBigFieldAsByteString() {
        return byteString(bigFieldDataIterator());
    }

    /**
     * Produces a BlockIterator that iterates over multiple big-field items appearing in a single expression, including any extension tokens, thus providing access to all the
     * big-field bytes.
     *
     * @return fully big-field-aware data iterator
     */
    public BlockIterator bigFieldDataIterator() {
        return new BlockIterator() {
            final OptIterator<ReadToken> it = iteratorToMarker();

            private BlockIterator internal = null;

            @Override
            public boolean hasNext() {
                if (internal != null && internal.hasNext()) {
                    return true;
                }
                for (Optional<ReadToken> opt = it.next(); opt.isPresent(); opt = it.next()) {
                    ReadToken token = opt.get();
                    if (Zchars.isBigField(token.getKey())) {
                        // note that the token's blockIterator handles extension blocks
                        internal = token.dataIterator();
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

            @Nonnull
            @Override
            public byte[] nextContiguous(int maxLength) {
                if (!hasNext()) {
                    throw new NoSuchElementException();
                }
                return internal.nextContiguous(maxLength);
            }

            @Nonnull
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
    public String toString() {
        return "ZscriptTokenExpression[" + iteratorToMarker().stream().map(Object::toString).collect(joining(", ")) + "]";
    }
}
