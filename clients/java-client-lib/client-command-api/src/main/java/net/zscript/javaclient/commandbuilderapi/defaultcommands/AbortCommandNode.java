package net.zscript.javaclient.commandbuilderapi.defaultcommands;

import javax.annotation.Nonnull;
import java.util.Map;

import static java.util.Collections.emptyList;

import net.zscript.javaclient.commandbuilderapi.nodes.CommandSequenceNode;
import net.zscript.javaclient.commandbuilderapi.nodes.ZscriptCommandNode;
import net.zscript.model.components.Zchars;
import net.zscript.model.components.ZscriptStatus;
import net.zscript.tokenizer.ZscriptExpression;

public class AbortCommandNode extends ZscriptCommandNode<DefaultResponse> {

    public AbortCommandNode() {
        super(null, emptyList(), Map.of(Zchars.Z_CMD, 1, Zchars.Z_STATUS, (int) ZscriptStatus.COMMAND_ERROR_CONTROL));
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
