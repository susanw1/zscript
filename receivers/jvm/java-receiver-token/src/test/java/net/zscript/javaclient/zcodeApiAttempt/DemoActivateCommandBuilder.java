package net.zscript.javaclient.zcodeApiAttempt;

import net.zscript.javaclient.zcodeApi.ZscriptCommandBuilder;
import net.zscript.javaclient.zcodeApi.ZscriptResponse;
import net.zscript.javaclient.zcodeApi.ZscriptUnparsedCommandResponse;
import net.zscript.javaclient.zcodeApiAttempt.DemoActivateCommandBuilder.DemoActivateCommandResponse;

public class DemoActivateCommandBuilder extends ZscriptCommandBuilder<DemoActivateCommandResponse> {
    public class DemoActivateCommandResponse implements ZscriptResponse {
        private final boolean alreadyActivated;

        public DemoActivateCommandResponse(boolean alreadyActivated) {
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
    protected DemoActivateCommandResponse parseResponse(ZscriptUnparsedCommandResponse resp) {
        return new DemoActivateCommandResponse(resp.getField('A').get() == 1 ? true : false);
    }

    @Override
    protected boolean commandCanFail() {
        return false;
    }
}
