package net.zscript.javaclient.commandbuilder.defaultCommands;

import net.zscript.javaclient.commandbuilder.commandnodes.CommandSequenceNode;
import net.zscript.javaclient.commandbuilder.commandnodes.ZscriptCommandNode;
import net.zscript.tokenizer.ZscriptExpression;

public class BlankCommandNode extends ZscriptCommandNode<DefaultResponse> {

    @Override
    public CommandSequenceNode thenFail() {
        return new FailureCommandNode();
    }

    @Override
    public CommandSequenceNode thenAbort() {
        return new AbortCommandNode();
    }

    @Override
    public CommandSequenceNode andThen(CommandSequenceNode next) {
        return next;
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
        return success -> failure -> success;
    }

    @Override
    public FailureConditional ifFails() {
        return failure -> success -> success;
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
    public DefaultResponse parseResponse(ZscriptExpression response) {
        return new DefaultResponse(response);
    }

    @Override
    public Class<DefaultResponse> getResponseType() {
        return DefaultResponse.class;
    }
}
