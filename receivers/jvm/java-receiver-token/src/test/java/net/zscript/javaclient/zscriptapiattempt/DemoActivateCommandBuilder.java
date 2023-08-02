package net.zscript.javaclient.zscriptapiattempt;

import net.zscript.javaclient.zscriptapi.ZscriptCommandBuilder;
import net.zscript.javaclient.zscriptapi.ZscriptResponse;
import net.zscript.javaclient.zscriptapi.ZscriptUnparsedCommandResponse;
import net.zscript.javaclient.zscriptapiattempt.DemoActivateCommandBuilder.DemoActivateCommandResponse;

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
