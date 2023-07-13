package org.zcode.javareceiver.modules.core;

import org.zcode.javareceiver.core.ZcodeCommandOutStream;
import org.zcode.javareceiver.core.ZcodeStatus;
import org.zcode.javareceiver.execution.ZcodeCommandContext;
import org.zcode.javareceiver.modules.ZcodeCommandFinder;

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

    public static int    userFirmwareVersion     = 1;
    public static String userFirmwareStr         = "";
    public static int    userHardwareVersion     = 1;
    public static String userHardwareStr         = "";
    public static int    platformFirmwareVersion = getVersion();
    public static String platformFirmwareStr     = "Java JRE " + System.getProperty("java.runtime.version");
    public static int    platformHardwareVersion = 0;
    public static String platformHardwareStr     = System.getProperty("os.arch") + " " + System.getProperty("os.name");
    public static int    coreZcodeVersion        = 0;
    public static String coreZcodeStr            = "Prototype parsing/running system for JVM";

    public static void execute(ZcodeCommandContext ctx) {
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
        out.writeField('M', ZcodeCommandFinder.getCommandSwitchExistsBottom(0));
    }

}
