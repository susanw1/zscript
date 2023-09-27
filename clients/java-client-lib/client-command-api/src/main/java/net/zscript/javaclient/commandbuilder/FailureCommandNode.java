package net.zscript.javaclient.commandbuilder;

import java.nio.charset.StandardCharsets;

public class FailureCommandNode extends ZscriptCommandNode {

    public static final byte[] ZSCRIPT_BYTES = "Z1S2".getBytes(StandardCharsets.UTF_8);

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
    byte[] compile(boolean includeParens) {
        return ZSCRIPT_BYTES;
    }
}
