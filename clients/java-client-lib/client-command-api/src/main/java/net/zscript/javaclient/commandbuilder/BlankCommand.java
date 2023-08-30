package net.zscript.javaclient.commandbuilder;

import net.zscript.javareceiver.tokenizer.ZscriptExpression;

public class BlankCommand extends ZscriptCommand {

    @Override
    public CommandSequence thenFail() {
        return new FailureCommand();
    }

    @Override
    public CommandSequence thenAbort() {
        return new AbortCommand();
    }

    @Override
    public CommandSequence andThen(CommandSequence next) {
        return next;
    }

    @Override
    public CommandSequence dropFailureCondition() {
        return this;
    }

    @Override
    public CommandSequence abortOnFail() {
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
    public CommandSequence onFail(CommandSequence next) {
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
    public CommandSequence reEvaluate() {
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
