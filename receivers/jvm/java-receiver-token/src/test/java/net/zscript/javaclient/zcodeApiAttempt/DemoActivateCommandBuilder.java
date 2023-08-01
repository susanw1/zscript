package net.zscript.javaclient.zcodeApiAttempt;

import net.zscript.javaclient.zcodeApi.ZcodeCommandBuilder;
import net.zscript.javaclient.zcodeApi.ZcodeResponse;
import net.zscript.javaclient.zcodeApi.ZcodeUnparsedCommandResponse;
import net.zscript.javaclient.zcodeApiAttempt.DemoActivateCommandBuilder.DemoActivateCommandResponse;

public class DemoActivateCommandBuilder extends ZcodeCommandBuilder<DemoActivateCommandResponse> {
    public class DemoActivateCommandResponse implements ZcodeResponse {
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
    protected DemoActivateCommandResponse parseResponse(ZcodeUnparsedCommandResponse resp) {
        return new DemoActivateCommandResponse(resp.getField('A').get() == 1 ? true : false);
    }

    @Override
    protected boolean commandCanFail() {
        return false;
    }
}
