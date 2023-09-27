package net.zscript.javaclient.commandbuilder;

import net.zscript.javareceiver.tokenizer.ZscriptExpression;

class DemoActivateCommand extends ZscriptBuiltCommandNode<DemoActivateCommand.DemoActivateCommandResponse> {
    //    private final boolean alreadyActivated;
    DemoActivateCommand(DemoActivateCommandBuilder builder) {
        super(builder);
    }

    @Override
    protected DemoActivateCommandResponse parseResponse(ZscriptExpression resp) {
        return new DemoActivateCommandResponse(resp, new byte[] {}, resp.getField('A').orElse(0) == 1);
    }

    @Override
    protected boolean canFail() {
        return false;
    }

    static class DemoActivateCommandBuilder extends ZscriptCommandBuilder<DemoActivateCommandResponse> {

        public DemoActivateCommandBuilder() {
            setField('Z', 2);
        }

        @Override
        public DemoActivateCommand build() {
            return new DemoActivateCommand(this);
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
