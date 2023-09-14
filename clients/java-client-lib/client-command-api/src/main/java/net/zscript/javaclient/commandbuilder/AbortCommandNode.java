package net.zscript.javaclient.commandbuilder;

import java.nio.charset.StandardCharsets;

import net.zscript.javareceiver.tokenizer.ZscriptExpression;

public class AbortCommandNode extends ZscriptCommandNode {

    @Override
    public CommandSequenceNode thenFail() {
        return this;
    }

    @Override
    public CommandSequenceNode thenAbort() {
        return this;
    }

    @Override
    public CommandSequenceNode andThen(CommandSequenceNode next) {
        return this;
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
        return a -> b -> this;
    }

    @Override
    public FailureConditional ifFails() {
        return a -> b -> this;
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
    public CommandSequenceNode reEvaluate() {
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
