package net.zscript.javaclient.commandbuilder;

import static java.nio.charset.StandardCharsets.ISO_8859_1;

import net.zscript.javareceiver.tokenizer.ZscriptExpression;

public class DemoCapabilitiesCommandBuilder extends ZscriptCommandBuilder<DemoCapabilitiesCommandResponse> {
    public static final int USER_FIRMWARE     = 0;
    public static final int USER_HARDWARE     = 1;
    public static final int PLATFORM_FIRMWARE = 2;
    public static final int PLATFORM_HARDWARE = 3;
    public static final int CORE_ZSCRIPT      = 4;

    public DemoCapabilitiesCommandBuilder() {
        setField('Z', 0);
    }

    public DemoCapabilitiesCommandBuilder setVersionType(int infoType) {
        setField('V', infoType);
        return this;
    }

    @Override
    public ZscriptBuiltCommandNode<DemoCapabilitiesCommandResponse> build() {
        return new DemoCapabilities(this);
    }
}

class DemoCapabilities extends ZscriptBuiltCommandNode<DemoCapabilitiesCommandResponse> {
    public DemoCapabilities(DemoCapabilitiesCommandBuilder builder) {
        super(builder);
    }

    public static DemoCapabilitiesCommandBuilder builder() {
        return new DemoCapabilitiesCommandBuilder();
    }

    @Override
    protected DemoCapabilitiesCommandResponse parseResponse(ZscriptExpression resp) {
        return new DemoCapabilitiesCommandResponse(resp, new byte[] {}, resp.getField('V').getAsInt(), new String(resp.getBigFieldData(), ISO_8859_1));
    }

    @Override
    protected boolean canFail() {
        return false;
    }
}

class DemoCapabilitiesCommandResponse extends ValidatingResponse {
    private       int    infoType = DemoCapabilitiesCommandBuilder.USER_FIRMWARE;
    private final int    version;
    private final String name;

    public DemoCapabilitiesCommandResponse(ZscriptExpression resp, byte[] bs, int version, String name) {
        super(resp, bs);
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
