package net.zscript.javaclient.commandbuilderapi.defaultcommands;

import java.util.OptionalInt;

import net.zscript.javaclient.commandbuilderapi.ZscriptResponse;
import net.zscript.tokenizer.ZscriptExpression;

public class DefaultResponse implements ZscriptResponse {
    private final ZscriptExpression expression;

    public DefaultResponse(ZscriptExpression expression) {
        this.expression = expression;
    }

    @Override
    public ZscriptExpression expression() {
        return expression;
    }

    @Override
    public boolean isValid() {
        return true;
    }

    @Override
    public OptionalInt getField(byte key) {
        return expression.getField(key);
    }
}
