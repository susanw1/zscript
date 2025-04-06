package net.zscript.javaclient.commandbuilderapi.nodes;

import javax.annotation.Nonnull;

import net.zscript.tokenizer.ZscriptExpression;
import net.zscript.util.ByteString;

/**
 * Test object - works like a generated capabilities command. Many methods exposed for testing.
 */
public class DemoCapabilitiesCommandBuilder extends ZscriptCommandBuilder<DemoCapabilitiesCommandResponse> {
    public static final int USER_FIRMWARE     = 0;
    public static final int USER_HARDWARE     = 1;
    public static final int PLATFORM_FIRMWARE = 2;
    public static final int PLATFORM_HARDWARE = 3;
    public static final int CORE_ZSCRIPT      = 4;

    public DemoCapabilitiesCommandBuilder() {
        // enforce 'V' for testing
        setRequiredFields(new byte[] { 'V' });
        setField('Z', 0);
    }

    public DemoCapabilitiesCommandBuilder setVersionType(int infoType) {
        setField('V', infoType);
        return this;
    }

    public ZscriptCommandBuilder<DemoCapabilitiesCommandResponse> setField(char key, int value) {
        return super.setField(key, value);
    }

    public ZscriptCommandBuilder<DemoCapabilitiesCommandResponse> setFieldAsText(char key, @Nonnull ByteString data) {
        return super.setFieldAsText((byte) key, data);
    }

    public ZscriptCommandBuilder<DemoCapabilitiesCommandResponse> setFieldAsBytes(char key, @Nonnull ByteString data) {
        return super.setFieldAsBytes((byte) key, data);
    }

    public ZscriptCommandBuilder<DemoCapabilitiesCommandResponse> setFieldAsSmallest(char key, @Nonnull ByteString data) {
        return super.setFieldAsSmallest((byte) key, data);
    }

    @Nonnull
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

    @Nonnull
    @Override
    public DemoCapabilitiesCommandResponse parseResponse(@Nonnull ZscriptExpression resp) {
        return new DemoCapabilitiesCommandResponse(resp, new byte[] {}, resp.getField('V').orElseThrow(), resp.getFieldAsByteString('I').orElseThrow().asString());
    }

    @Nonnull
    @Override
    public Class<DemoCapabilitiesCommandResponse> getResponseType() {
        return DemoCapabilitiesCommandResponse.class;
    }

    @Override
    public boolean canFail() {
        return false;
    }
}
