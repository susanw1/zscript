package net.zscript.javaclient.commandbuilderapi;

import javax.annotation.Nonnull;

import net.zscript.javaclient.commandbuilderapi.nodes.ZscriptCommandBuilder;
import net.zscript.javaclient.commandbuilderapi.nodes.ZscriptCommandNode;
import net.zscript.tokenizer.ZscriptExpression;

public class DemoActivateCommand extends ZscriptCommandNode<DemoActivateCommand.DemoActivateCommandResponse> {
    //    private final boolean alreadyActivated;
    DemoActivateCommand(DemoActivateCommandBuilder builder) {
        super(builder);
    }

    public static DemoActivateCommandBuilder builder() {
        return new DemoActivateCommandBuilder();
    }

    @Nonnull
    @Override
    public DemoActivateCommandResponse parseResponse(@Nonnull ZscriptExpression resp) {
        return new DemoActivateCommandResponse(resp, new byte[] {}, resp.getField('A').orElse(0) == 1);
    }

    @Nonnull
    @Override
    public Class<DemoActivateCommandResponse> getResponseType() {
        return DemoActivateCommandResponse.class;
    }

    @Override
    public boolean canFail() {
        return true;
    }

    public static class DemoActivateCommandBuilder extends ZscriptCommandBuilder<DemoActivateCommandResponse> {

        public DemoActivateCommandBuilder() {
            setField('Z', 2);
        }

        public DemoActivateCommandBuilder setChallenge(int challenge) {
            setField('K', challenge);
            return this;
        }

        @Nonnull
        @Override
        public DemoActivateCommand build() {
            return new DemoActivateCommand(this);
        }

        public DemoActivateCommandBuilder setField(char key, int value) {
            return (DemoActivateCommandBuilder) super.setField(key, value);
        }
    }

    public static class DemoActivateCommandResponse extends ValidatingResponse {
        private final boolean alreadyActivated;

        public DemoActivateCommandResponse(ZscriptExpression resp, byte[] requiredKeys, boolean alreadyActivated) {
            super(resp, requiredKeys);
            this.alreadyActivated = alreadyActivated;
        }

        public boolean alreadyActivated() {
            return alreadyActivated;
        }
    }
}
