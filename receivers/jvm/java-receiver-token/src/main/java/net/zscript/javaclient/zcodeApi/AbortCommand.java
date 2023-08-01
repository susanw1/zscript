package net.zscript.javaclient.zcodeApi;

import java.nio.charset.StandardCharsets;

public class AbortCommand extends ZscriptCommand {

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
        return this;
    }

    @Override
    public CommandSeqElement abortOnFail() {
        return this;
    }

    @Override
    public SuccessConditional ifSucceeds() {
        return a -> b -> this;
    }

    @Override
    public FailureConditional ifFails() {
        return a -> b -> this;
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
    public CommandSeqElement reEvaluate() {
        return this;
    }

    @Override
    public byte[] compile(boolean includeParens) {
        return "Z1S13".getBytes(StandardCharsets.UTF_8);
    }

    @Override
    public boolean isCommand() {
        return true;
    }

    @Override
    public void response(ZscriptUnparsedCommandResponse resp) {
    }

    @Override
    public void notExecuted() {
    }
}
