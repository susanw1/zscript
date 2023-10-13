package net.zscript.javaclient.commandbuilder;

import java.util.OptionalInt;

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
        return true;
        //return expression.isValid(requiredKeys);
    }

    @Override
    public OptionalInt getField(char key) {
        return expression.getField(key);
    }

}
