package net.zscript.javaclient.zscriptapiattempt;

import net.zscript.javaclient.zscriptapi.ZscriptCommandBuilder;
import net.zscript.javaclient.zscriptapi.ZscriptResponse;
import net.zscript.javaclient.zscriptapi.ZscriptUnparsedCommandResponse;
import net.zscript.javaclient.zscriptapiattempt.DemoCapabilitiesCommandBuilder.DemoCapabilitiesCommandResponse;

public class DemoCapabilitiesCommandBuilder extends ZscriptCommandBuilder<DemoCapabilitiesCommandResponse> {
    public static final int USER_FIRMWARE     = 0;
    public static final int USER_HARDWARE     = 1;
    public static final int PLATFORM_FIRMWARE = 2;
    public static final int PLATFORM_HARDWARE = 3;
    public static final int CORE_ZSCRIPT      = 4;

    private int infoType = USER_FIRMWARE;

    public class DemoCapabilitiesCommandResponse implements ZscriptResponse {
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
    protected DemoCapabilitiesCommandResponse parseResponse(ZscriptUnparsedCommandResponse resp) {
        return new DemoCapabilitiesCommandResponse(resp.getField('V').get(), resp.getBigFieldString());
    }

    @Override
    protected boolean commandCanFail() {
        return false;
    }

}
