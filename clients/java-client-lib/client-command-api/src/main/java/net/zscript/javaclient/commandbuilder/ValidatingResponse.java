package net.zscript.javaclient.commandbuilder;

import net.zscript.javareceiver.tokenizer.Zchars;
import net.zscript.javareceiver.tokenizer.ZscriptExpression;

public abstract class ValidatingResponse implements ZscriptResponse {
    protected final ZscriptExpression expression;
    private final byte[]              requiredKeys;

    public ValidatingResponse(final ZscriptExpression expression, final byte[] requiredKeys) {
        this.expression = expression;
        this.requiredKeys = requiredKeys;
    }

    public ZscriptExpression getExpression() {
        return expression;
    }

    @Override
    public boolean isValid() {
        for (final byte b : requiredKeys) {
            if (Zchars.isNumericKey(b) && !expression.hasField(b) || Zchars.isBigField(b) && !expression.hasBigField()) {
                return false;
            }
        }
        return true;
    }
}