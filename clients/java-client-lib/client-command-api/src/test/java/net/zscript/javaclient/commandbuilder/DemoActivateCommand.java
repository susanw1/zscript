package net.zscript.javaclient.commandbuilder;

import net.zscript.javaclient.commandbuilder.commandnodes.ZscriptCommandBuilder;
import net.zscript.javaclient.commandbuilder.commandnodes.ZscriptCommandNode;
import net.zscript.tokenizer.ZscriptExpression;

public class DemoActivateCommand extends ZscriptCommandNode<DemoActivateCommand.DemoActivateCommandResponse> {
    //    private final boolean alreadyActivated;
    DemoActivateCommand(DemoActivateCommandBuilder builder) {
        super(builder);
    }

    public static DemoActivateCommandBuilder builder() {
        return new DemoActivateCommandBuilder();
    }

    @Override
    public DemoActivateCommandResponse parseResponse(ZscriptExpression resp) {
        return new DemoActivateCommandResponse(resp, new byte[] {}, resp.getField('A').orElse(0) == 1);
    }

    @Override
    public Class<DemoActivateCommandResponse> getResponseType() {
        return DemoActivateCommandResponse.class;
    }

    @Override
    public boolean canFail() {
        return false;
    }

    public static class DemoActivateCommandBuilder extends ZscriptCommandBuilder<DemoActivateCommandResponse> {

        public DemoActivateCommandBuilder() {
            setField('Z', 2);
        }

        @Override
        public DemoActivateCommand build() {
            return new DemoActivateCommand(this);
        }

        public ZscriptCommandBuilder<DemoActivateCommandResponse> setField(char key, int value) {
            return super.setField(key, value);
        }
    }

    static class DemoActivateCommandResponse extends ValidatingResponse {
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
