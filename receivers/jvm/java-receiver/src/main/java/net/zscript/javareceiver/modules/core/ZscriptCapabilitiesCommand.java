package net.zscript.javareceiver.modules.core;

import net.zscript.javareceiver.core.CommandOutStream;
import net.zscript.javareceiver.execution.CommandContext;
import net.zscript.model.components.ZscriptStatus;

public class ZscriptCapabilitiesCommand {

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

    public final int    userFirmwareVersion     = 1;
    public final String userFirmwareStr         = "";
    public final int    userHardwareVersion     = 1;
    public final String userHardwareStr         = "";
    public final int    platformFirmwareVersion = getVersion();
    public final String platformFirmwareStr     = "Java JRE " + System.getProperty("java.runtime.version");
    public final int    platformHardwareVersion = 0;
    public final String platformHardwareStr     = System.getProperty("os.arch") + " " + System.getProperty("os.name");
    public final int    coreZscriptVersion      = 0;
    public final String coreZscriptStr          = "Prototype parsing/running system for JVM";

    public void execute(CommandContext ctx) {
        CommandOutStream out = ctx.getOutStream();

        int versionType = ctx.getFieldValueOrDefault((byte) 'V', 0);

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
            str = coreZscriptStr;
            ver = coreZscriptVersion;
            break;
        }
        if (str != null) {
            out.writeField('V', ver);
            out.writeFieldQuoted('I', str);
        } else {
            ctx.status(ZscriptStatus.VALUE_OUT_OF_RANGE);
        }
        out.writeField('C', ZscriptCoreModule.getCommands());
        out.writeField('M', ctx.getZscript().getModuleRegistry().getCommandSwitchExistsBottom(0));
    }

}
