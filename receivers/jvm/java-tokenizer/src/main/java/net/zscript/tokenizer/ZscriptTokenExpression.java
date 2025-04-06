package net.zscript.tokenizer;

import javax.annotation.Nonnull;
import java.util.Optional;
import java.util.function.Supplier;
import java.util.stream.Stream;

import static java.util.stream.Collectors.joining;

import net.zscript.model.components.Zchars;
import net.zscript.tokenizer.TokenBuffer.TokenReader.ReadToken;
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
    public Stream<ZscriptTokenField> fields() {
        return iteratorToMarker().stream()
                .filter(tok -> Zchars.isExpressionKey(tok.getKey()))
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
                .filter(tok -> Zchars.isExpressionKey(tok.getKey()))
                .count();
    }

    @Override
    public String toString() {
        return "ZscriptTokenExpression[" + iteratorToMarker().stream().map(Object::toString).collect(joining(", ")) + "]";
    }
}
