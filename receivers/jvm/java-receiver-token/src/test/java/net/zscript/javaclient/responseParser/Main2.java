package net.zscript.javaclient.responseParser;

import net.zscript.javaclient.responseParser.ZscriptAddress;
import net.zscript.javaclient.responseParser.CommandResponseQueue;
import net.zscript.javaclient.responseParser.CommandResponseSystem;
import net.zscript.javaclient.zcodeApiAttempt.DemoActivateCommandBuilder;
import net.zscript.javaclient.zcodeApiAttempt.DemoCapabilitiesCommandBuilder;

public class Main2 {
    public static void main(String[] args) {
        CommandResponseSystem zcodeOut = new CommandResponseQueue(new LocalZscriptConnection());
        zcodeOut.send(new DemoCapabilitiesCommandBuilder()
                .setVersionType(DemoCapabilitiesCommandBuilder.PLATFORM_FIRMWARE)
                .addResponseListener(r -> System.out.println(r.getName()))
                .build()
                .andThen(new DemoActivateCommandBuilder()
                        .addResponseListener(r -> System.out.println(r.alreadyActivated()))
                        .build())
                .andThen(new DemoActivateCommandBuilder()
                        .addResponseListener(r -> System.out.println(r.alreadyActivated()))
                        .build()));
        CommandResponseSystem zcodeOutAddr = new CommandResponseQueue(
                zcodeOut.getResponseAddressingSystem().getAddressConnection(ZscriptAddress.from(0x50, 0x0, 0x1)));
        zcodeOutAddr.send(new DemoCapabilitiesCommandBuilder()
                .setVersionType(DemoCapabilitiesCommandBuilder.PLATFORM_FIRMWARE)
                .addResponseListener(r -> System.out.println(r.getName()))
                .build());
    }
}
