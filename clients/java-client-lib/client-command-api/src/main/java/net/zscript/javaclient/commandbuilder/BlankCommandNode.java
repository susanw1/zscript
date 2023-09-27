package net.zscript.javaclient.commandbuilder;

import net.zscript.javareceiver.tokenizer.ZscriptExpression;

public class BlankCommandNode extends ZscriptCommandNode {

    public static final byte[] ZSCRIPT_BYTES = new byte[0];

    @Override
    public CommandSequenceNode thenFail() {
        return new FailureCommandNode();
    }

    @Override
    public CommandSequenceNode thenAbort() {
        return new AbortCommandNode();
    }

    @Override
    public CommandSequenceNode andThen(CommandSequenceNode next) {
        return next;
    }

    @Override
    public CommandSequenceNode dropFailureCondition() {
        return this;
    }

    @Override
    public CommandSequenceNode abortOnFail() {
        return this;
    }

    @Override
    public SuccessConditional ifSucceeds() {
        return success -> failure -> success;
    }

    @Override
    public FailureConditional ifFails() {
        return failure -> success -> success;
    }

    @Override
    public CommandSequenceNode onFail(CommandSequenceNode next) {
        return this;
    }

    @Override
    protected boolean canFail() {
        return false;
    }

    @Override
    byte[] compile(boolean includeParens) {
        return ZSCRIPT_BYTES;
    }

    @Override
    public void onResponse(ZscriptExpression resp) {
    }

    @Override
    public void onNotExecuted() {
    }
}
