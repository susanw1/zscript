package net.zscript.javaclient.commandbuilderapi.nodes;

import net.zscript.javaclient.commandbuilderapi.ValidatingResponse;
import net.zscript.tokenizer.ZscriptExpression;

/**
 * Test object - works like a generated capabilities command's response.
 */
public class DemoCapabilitiesCommandResponse extends ValidatingResponse {
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
