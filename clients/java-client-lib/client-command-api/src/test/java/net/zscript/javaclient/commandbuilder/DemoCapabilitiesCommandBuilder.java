package net.zscript.javaclient.commandbuilder;

import static java.nio.charset.StandardCharsets.ISO_8859_1;

import net.zscript.javaclient.commandbuilder.DemoCapabilitiesCommandBuilder.DemoCapabilitiesCommandResponse;
import net.zscript.javareceiver.tokenizer.ZscriptExpression;

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
    protected DemoCapabilitiesCommandResponse parseResponse(ZscriptExpression resp) {
        return new DemoCapabilitiesCommandResponse(resp.getField('V').getAsInt(), new String(resp.getBigFieldData(), ISO_8859_1));
    }

    @Override
    protected boolean commandCanFail() {
        return false;
    }
}
