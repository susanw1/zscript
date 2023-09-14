package net.zscript.javaclient.commandbuilder;

import net.zscript.javareceiver.tokenizer.ZscriptExpression;

public class BlankCommandNode extends ZscriptCommandNode {

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
    public boolean canFail() {
        return false;
    }

    @Override
    public boolean isCommand() {
        return true;
    }

    @Override
    public CommandSequenceNode reEvaluate() {
        return this;
    }

    @Override
    public byte[] compile(boolean includeParens) {
        return new byte[0];
    }

    @Override
    public void onResponse(ZscriptExpression resp) {
    }

    @Override
    public void onNotExecuted() {
    }
}
