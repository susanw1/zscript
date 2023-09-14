package net.zscript.javaclient.commandbuilder;

import java.nio.charset.StandardCharsets;

import net.zscript.javareceiver.tokenizer.ZscriptExpression;

public class FailureCommandNode extends ZscriptCommandNode {

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
        return new BlankCommandNode();
    }

    @Override
    public CommandSequenceNode abortOnFail() {
        return new AbortCommandNode();
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
    public CommandSequenceNode onFail(CommandSequenceNode next) {
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
    public CommandSequenceNode reEvaluate() {
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
