package net.zscript.javaclient.commandbuilderapi;

import java.util.OptionalInt;

import net.zscript.model.components.Zchars;
import net.zscript.tokenizer.ZscriptExpression;

public class ValidatingResponse implements ZscriptResponse {
    protected final ZscriptExpression expression;
    private final   byte[]            requiredKeys;

    public ValidatingResponse(final ZscriptExpression expression, final byte[] requiredKeys) {
        this.expression = expression;
        this.requiredKeys = requiredKeys;
    }

    @Override
    public ZscriptExpression expression() {
        return expression;
    }

    /**
     * {@inheritDoc}
     *
     * Note that at this time, it validates <em>required</em> fields, but not whether they are in a valid range.
     */
    @Override
    public boolean isValid() {
        for (byte b : requiredKeys) {
            if (Zchars.isExpressionKey(b) && !expression.hasField(b)) {
                return false;
            }
        }
        return true;
    }

    @Override
    public OptionalInt getFieldValue(byte key) {
        return expression.getFieldValue(key);
    }
}
