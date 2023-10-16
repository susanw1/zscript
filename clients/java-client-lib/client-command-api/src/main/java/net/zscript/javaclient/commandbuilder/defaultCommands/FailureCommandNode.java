package net.zscript.javaclient.commandbuilder.defaultCommands;

import java.nio.charset.StandardCharsets;

import net.zscript.javaclient.commandbuilder.commandnodes.CommandSequenceNode;
import net.zscript.javaclient.commandbuilder.commandnodes.ZscriptCommandNode;
import net.zscript.javareceiver.tokenizer.ZscriptExpression;

public class FailureCommandNode extends ZscriptCommandNode<DefaultResponse> {

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
    public DefaultResponse parseResponse(ZscriptExpression response) {
        return new DefaultResponse(response);
    }

    @Override
    public Class<DefaultResponse> getResponseType() {
        return DefaultResponse.class;
    }
}
