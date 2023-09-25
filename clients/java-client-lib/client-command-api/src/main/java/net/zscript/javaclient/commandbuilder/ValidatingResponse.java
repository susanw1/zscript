package net.zscript.javaclient.commandbuilder;

import java.util.BitSet;
import java.util.EnumSet;
import java.util.OptionalInt;
import java.util.stream.Collectors;

import net.zscript.javareceiver.tokenizer.BlockIterator;
import net.zscript.javareceiver.tokenizer.ZscriptExpression;

public abstract class ValidatingResponse implements ZscriptResponse {
    protected final ZscriptExpression expression;
    private final   byte[]            requiredKeys;

    public ValidatingResponse(final ZscriptExpression expression, final byte[] requiredKeys) {
        this.expression = expression;
        this.requiredKeys = requiredKeys;
    }

    public ZscriptExpression getExpression() {
        return expression;
    }

    /**
     * {@inheritDoc}
     *
     * Note that at this time, it validates <em>required</em> fields, but not whether they are in a valid range.
     */
    @Override
    public boolean isValid() {
        return expression.isValid(requiredKeys);
    }

    @Override
    public OptionalInt getField(char key) {
        return expression.getField(key);
    }

    @Override
    public BlockIterator getBigField() {
        return expression.getBigField();
    }

    protected static <E extends Enum<E>> EnumSet<E> bitsetToEnumSet(int bitFields, Class<E> enumClass) {
        final E[] enumValues = enumClass.getEnumConstants();
        return BitSet.valueOf(new long[] { bitFields }).stream()
                .mapToObj(b -> enumValues[b])
                .collect(Collectors.toCollection(() -> EnumSet.noneOf(enumClass)));
    }

    protected static <E extends Enum<E>> int checkInEnumRange(int value, Class<E> enumClass, char key, boolean isBitset) {
        return ZscriptCommandBuilder.checkInEnumRange(value, enumClass, key, isBitset);
    }
}
