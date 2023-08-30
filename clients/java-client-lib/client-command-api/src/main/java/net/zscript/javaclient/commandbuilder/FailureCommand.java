package net.zscript.javaclient.commandbuilder;

import java.nio.charset.StandardCharsets;

import net.zscript.javareceiver.tokenizer.ZscriptExpression;

public class FailureCommand extends ZscriptCommand {

    @Override
    public CommandSequence thenFail() {
        return this;
    }

    @Override
    public CommandSequence thenAbort() {
        return this;
    }

    @Override
    public CommandSequence andThen(CommandSequence next) {
        return this;
    }

    @Override
    public CommandSequence dropFailureCondition() {
        return new BlankCommand();
    }

    @Override
    public CommandSequence abortOnFail() {
        return new AbortCommand();
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
    public CommandSequence onFail(CommandSequence next) {
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
    public CommandSequence reEvaluate() {
        return this;
    }

    @Override
    public byte[] compile(boolean includeParens) {
        return "Z1S2".getBytes(StandardCharsets.UTF_8);
    }

    @Override
    public void onResponse(ZscriptExpression resp) {
    }

    @Override
    public void onNotExecuted() {
    }
}
