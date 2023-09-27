package net.zscript.javaclient.commandbuilder;

import java.nio.charset.StandardCharsets;

public class AbortCommandNode extends ZscriptCommandNode {
    private static final byte[] ZSCRIPT_BYTES = "Z1S13".getBytes(StandardCharsets.UTF_8);

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
    protected boolean canFail() {
        return false;
    }

    @Override
    byte[] compile(boolean includeParens) {
        return ZSCRIPT_BYTES;
    }
}
