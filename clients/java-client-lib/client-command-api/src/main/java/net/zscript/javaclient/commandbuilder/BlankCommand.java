package net.zscript.javaclient.commandbuilder;

import net.zscript.javareceiver.tokenizer.ZscriptExpression;

public class BlankCommand extends ZscriptCommand {

    @Override
    public CommandSeqElement thenFail() {
        return new FailureCommand();
    }

    @Override
    public CommandSeqElement thenAbort() {
        return new AbortCommand();
    }

    @Override
    public CommandSeqElement andThen(CommandSeqElement next) {
        return next;
    }

    @Override
    public CommandSeqElement dropFailureCondition() {
        return this;
    }

    @Override
    public CommandSeqElement abortOnFail() {
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
    public CommandSeqElement onFail(CommandSeqElement next) {
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
    public CommandSeqElement reEvaluate() {
        return this;
    }

    @Override
    public byte[] compile(boolean includeParens) {
        return new byte[0];
    }

    @Override
    public void response(ZscriptExpression resp) {
    }

    @Override
    public void notExecuted() {
    }
}
