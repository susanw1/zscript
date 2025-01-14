package net.zscript.javaclient.commandbuilder.defaultcommands;

import javax.annotation.Nonnull;
import java.util.Collections;
import java.util.Map;

import net.zscript.javaclient.commandbuilder.nodes.CommandSequenceNode;
import net.zscript.javaclient.commandbuilder.nodes.ZscriptCommandNode;
import net.zscript.model.components.Zchars;
import net.zscript.model.components.ZscriptStatus;
import net.zscript.tokenizer.ZscriptExpression;

public class FailureCommandNode extends ZscriptCommandNode<DefaultResponse> {

    public FailureCommandNode() {
        super(null, Collections.emptyList(), Map.of(Zchars.Z_CMD, 1, Zchars.Z_STATUS, (int) ZscriptStatus.COMMAND_FAIL_CONTROL));
    }

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

    @Nonnull
    @Override
    public DefaultResponse parseResponse(ZscriptExpression response) {
        return new DefaultResponse(response);
    }

    @Nonnull
    @Override
    public Class<DefaultResponse> getResponseType() {
        return DefaultResponse.class;
    }
}
