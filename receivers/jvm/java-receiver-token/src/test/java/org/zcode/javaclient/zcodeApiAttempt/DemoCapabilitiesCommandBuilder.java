package org.zcode.javaclient.zcodeApiAttempt;

import org.zcode.javaclient.zcodeApi.ZcodeCommandBuilder;
import org.zcode.javaclient.zcodeApi.ZcodeResponse;
import org.zcode.javaclient.zcodeApi.ZcodeUnparsedCommandResponse;
import org.zcode.javaclient.zcodeApiAttempt.DemoCapabilitiesCommandBuilder.DemoCapabilitiesCommandResponse;

public class DemoCapabilitiesCommandBuilder extends ZcodeCommandBuilder<DemoCapabilitiesCommandResponse> {
    public static final int USER_FIRMWARE     = 0;
    public static final int USER_HARDWARE     = 1;
    public static final int PLATFORM_FIRMWARE = 2;
    public static final int PLATFORM_HARDWARE = 3;
    public static final int CORE_ZCODE        = 4;

    private int infoType = USER_FIRMWARE;

    public class DemoCapabilitiesCommandResponse implements ZcodeResponse {
        private final int    version;
        private final String name;

        public DemoCapabilitiesCommandResponse(int version, String name) {
            this.version = version;
            this.name = name;
        }

        public int getVersion() {
            return version;
        }

        public String getName() {
            return name;
        }

        public int getVersionType() {
            return infoType;
        }
    }

    public DemoCapabilitiesCommandBuilder() {
        setField('Z', 0);
    }

    public DemoCapabilitiesCommandBuilder setVersionType(int infoType) {
        this.infoType = infoType;
        setField('V', infoType);
        return this;
    }

    @Override
    protected DemoCapabilitiesCommandResponse parseResponse(ZcodeUnparsedCommandResponse resp) {
        return new DemoCapabilitiesCommandResponse(resp.getField('V').get(), resp.getBigFieldString());
    }

    @Override
    protected boolean commandCanFail() {
        return false;
    }

}