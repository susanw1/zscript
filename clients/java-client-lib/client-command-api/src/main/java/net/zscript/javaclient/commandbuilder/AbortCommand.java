package net.zscript.javaclient.commandbuilder;

import java.nio.charset.StandardCharsets;

import net.zscript.javareceiver.tokenizer.ZscriptExpression;

public class AbortCommand extends ZscriptCommand {

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
        return this;
    }

    @Override
    public CommandSequence abortOnFail() {
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
    public CommandSequence onFail(CommandSequence next) {
        return this;
    }

    @Override
    public boolean canFail() {
        return false;
    }

    @Override
    public CommandSequence reEvaluate() {
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
    public void onResponse(ZscriptExpression resp) {
    }

    @Override
    public void onNotExecuted() {
    }
}
