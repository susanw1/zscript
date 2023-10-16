package net.zscript.javaclient.commandbuilder.defaultCommands;

import java.nio.charset.StandardCharsets;
import java.util.Collections;
import java.util.Map;

import net.zscript.javaclient.commandbuilder.commandnodes.CommandSequenceNode;
import net.zscript.javaclient.commandbuilder.commandnodes.ZscriptCommandNode;
import net.zscript.javareceiver.tokenizer.ZscriptExpression;
import net.zscript.model.components.Zchars;
import net.zscript.model.components.ZscriptStatus;

public class AbortCommandNode extends ZscriptCommandNode<DefaultResponse> {

    public AbortCommandNode() {
        super(null, Collections.emptyList(), Map.of(Zchars.Z_CMD, 1, Zchars.Z_STATUS, (int) ZscriptStatus.COMMAND_ERROR_CONTROL));
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
