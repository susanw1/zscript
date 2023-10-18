package net.zscript.javaclient.commandbuilder.commandNodes;

import static java.nio.charset.StandardCharsets.ISO_8859_1;

import net.zscript.javaclient.commandbuilder.ValidatingResponse;
import net.zscript.javaclient.commandbuilder.commandnodes.ZscriptCommandBuilder;
import net.zscript.javaclient.commandbuilder.commandnodes.ZscriptCommandNode;
import net.zscript.javaclient.commandbuilder.defaultCommands.DefaultResponse;
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

    public ZscriptCommandBuilder<DemoCapabilitiesCommandResponse> setField(char key, int value) {
        return super.setField(key, value);
    }

    public ZscriptCommandBuilder<DemoCapabilitiesCommandResponse> addBigField(byte[] data) {
        return super.addBigField(data);
    }

    public ZscriptCommandBuilder<DemoCapabilitiesCommandResponse> addBigField(byte[] data, boolean asString) {
        return super.addBigField(data, asString);
    }

    public ZscriptCommandBuilder<DemoCapabilitiesCommandResponse> addBigFieldAsSmallest(byte[] data) {
        return super.addBigField(data);
    }

    public ZscriptCommandBuilder<DemoCapabilitiesCommandResponse> addBigField(String data) {
        return super.addBigField(data);
    }

    @Override
    public ZscriptCommandNode<DemoCapabilitiesCommandResponse> build() {
        return new DemoCapabilities(this);
    }
}

class DemoCapabilities extends ZscriptCommandNode<DemoCapabilitiesCommandResponse> {
    public DemoCapabilities(DemoCapabilitiesCommandBuilder builder) {
        super(builder);
    }

    public static DemoCapabilitiesCommandBuilder builder() {
        return new DemoCapabilitiesCommandBuilder();
    }

    @Override
    public DemoCapabilitiesCommandResponse parseResponse(ZscriptExpression resp) {
        return new DemoCapabilitiesCommandResponse(resp, new byte[] {}, resp.getField('V').getAsInt(), new String(resp.getBigFieldData(), ISO_8859_1));
    }

    @Override
    public Class<DemoCapabilitiesCommandResponse> getResponseType() {
        return DemoCapabilitiesCommandResponse.class;
    }

    @Override
    public boolean canFail() {
        return false;
    }
}

