package net.zscript.javaclient.commandbuilder.commandNodes;

import net.zscript.javaclient.commandbuilder.ValidatingResponse;
import net.zscript.tokenizer.ZscriptExpression;

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
