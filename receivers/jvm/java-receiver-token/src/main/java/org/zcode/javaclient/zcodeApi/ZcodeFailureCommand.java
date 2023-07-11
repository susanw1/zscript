package org.zcode.javaclient.zcodeApi;

import java.nio.charset.StandardCharsets;

public class ZcodeFailureCommand extends ZcodeCommand {

    @Override
    public CommandSeqElement thenFail() {
        return this;
    }

    @Override
    public CommandSeqElement thenAbort() {
        return this;
    }

    @Override
    public CommandSeqElement andThen(CommandSeqElement next) {
        return this;
    }

    @Override
    public CommandSeqElement dropFailureCondition() {
        return new ZcodeBlankCommand();
    }

    @Override
    public CommandSeqElement abortOnFail() {
        return new ZcodeAbortCommand();
    }

    @Override
    public SuccessConditional ifSucceeds() {
        return success -> failure -> failure;
    }

    @Override
    public FailureConditional ifFails() {
        return failure -> success -> failure;
    }

    @Override
    public CommandSeqElement onFail(CommandSeqElement next) {
        return next;
    }

    @Override
    public boolean canFail() {
        return true;
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
        return "Z1S10".getBytes(StandardCharsets.UTF_8);
    }

    @Override
    public void response(ZcodeUnparsedCommandResponse resp) {
    }

}
