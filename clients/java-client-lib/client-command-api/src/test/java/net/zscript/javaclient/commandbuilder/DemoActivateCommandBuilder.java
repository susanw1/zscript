package net.zscript.javaclient.commandbuilder;

import net.zscript.javaclient.commandbuilder.DemoActivateCommandBuilder.DemoActivateCommandResponse;
import net.zscript.javareceiver.tokenizer.ZscriptExpression;

public class DemoActivateCommandBuilder extends ZscriptCommandBuilder<DemoActivateCommandResponse> {
    public class DemoActivateCommandResponse extends ValidatingResponse {
        private final boolean alreadyActivated;

        public DemoActivateCommandResponse(ZscriptExpression resp, byte[] requiredKeys, boolean alreadyActivated) {
            super(resp, requiredKeys);
            this.alreadyActivated = alreadyActivated;
        }

        public boolean alreadyActivated() {
            return alreadyActivated;
        }
    }

    public DemoActivateCommandBuilder() {
        setField('Z', 2);
    }

    @Override
    protected DemoActivateCommandResponse parseResponse(ZscriptExpression resp) {
        return new DemoActivateCommandResponse(resp, new byte[] {}, resp.getField('A').getAsInt() == 1 ? true : false);
    }

    @Override
    protected boolean commandCanFail() {
        return false;
    }
}
