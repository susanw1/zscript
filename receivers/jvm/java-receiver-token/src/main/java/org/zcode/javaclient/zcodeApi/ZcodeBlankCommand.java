package org.zcode.javaclient.zcodeApi;

public class ZcodeBlankCommand extends ZcodeCommand {

    @Override
    public CommandSeqElement thenFail() {
        return new ZcodeFailureCommand();
    }

    @Override
    public CommandSeqElement thenAbort() {
        return new ZcodeAbortCommand();
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
    public void response(ZcodeUnparsedCommandResponse resp) {
    }

    @Override
    public void notExecuted() {
    }
}
