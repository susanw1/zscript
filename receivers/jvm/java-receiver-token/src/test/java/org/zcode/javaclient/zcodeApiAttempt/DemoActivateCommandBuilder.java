package org.zcode.javaclient.zcodeApiAttempt;

import org.zcode.javaclient.zcodeApi.ZcodeCommandBuilder;
import org.zcode.javaclient.zcodeApi.ZcodeResponse;
import org.zcode.javaclient.zcodeApi.ZcodeUnparsedCommandResponse;

public class DemoActivateCommandBuilder extends ZcodeCommandBuilder<org.zcode.javaclient.zcodeApiAttempt.DemoActivateCommandBuilder.DemoActivateCommandResponse> {
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
