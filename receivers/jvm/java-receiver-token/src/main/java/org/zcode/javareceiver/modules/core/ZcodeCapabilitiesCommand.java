package org.zcode.javareceiver.modules.core;

import org.zcode.javareceiver.core.ZcodeCommandOutStream;
import org.zcode.javareceiver.core.ZcodeStatus;
import org.zcode.javareceiver.execution.ZcodeCommandContext;

public class ZcodeCapabilitiesCommand {

    private static int getVersion() {
        String version = System.getProperty("java.version");
        if (version.startsWith("1.")) {
            version = version.substring(2, 3);
        } else {
            int dot = version.indexOf(".");
            if (dot != -1) {
                version = version.substring(0, dot);
            }
        }
        return Integer.parseInt(version);
    }

    public int    userFirmwareVersion     = 1;
    public String userFirmwareStr         = "";
    public int    userHardwareVersion     = 1;
    public String userHardwareStr         = "";
    public int    platformFirmwareVersion = getVersion();
    public String platformFirmwareStr     = "Java JRE " + System.getProperty("java.runtime.version");
    public int    platformHardwareVersion = 0;
    public String platformHardwareStr     = System.getProperty("os.arch") + " " + System.getProperty("os.name");
    public int    coreZcodeVersion        = 0;
    public String coreZcodeStr            = "Prototype parsing/running system for JVM";

    public void execute(ZcodeCommandContext ctx) {
        ZcodeCommandOutStream out = ctx.getOutStream();

        int versionType = ctx.getField((byte) 'V', 0);

        String str = null;
        int    ver = 0;
        switch (versionType) {
        case 0:
            str = userFirmwareStr;
            ver = userFirmwareVersion;
            break;
        case 1:
            str = userHardwareStr;
            ver = userHardwareVersion;
            break;
        case 2:
            str = platformFirmwareStr;
            ver = platformFirmwareVersion;
            break;
        case 3:
            str = platformHardwareStr;
            ver = platformHardwareVersion;
            break;
        case 4:
            str = coreZcodeStr;
            ver = coreZcodeVersion;
            break;
        }
        if (str != null) {
            out.writeField('V', ver);
            if (str.length() > 0) {
                out.writeString(str);
            }
        } else {
            ctx.status(ZcodeStatus.VALUE_OUT_OF_RANGE);
        }
        out.writeField('C', ZcodeCoreModule.getCommands());
        out.writeField('M', ctx.getZcode().getModuleFinder().getCommandSwitchExistsBottom(0));
    }

}
