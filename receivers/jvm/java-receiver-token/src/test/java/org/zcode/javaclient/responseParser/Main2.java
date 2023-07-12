package org.zcode.javaclient.responseParser;

import org.zcode.javaclient.zcodeApiAttempt.DemoActivateCommandBuilder;
import org.zcode.javaclient.zcodeApiAttempt.DemoCapabilitiesCommandBuilder;

public class Main2 {
    public static void main(String[] args) {
        ZcodeCommandResponseSystem zcodeOut = new ZcodeCommandResponseQueue(new LocalZcodeConnection());
        zcodeOut.send(new DemoCapabilitiesCommandBuilder()
                .setVersionType(DemoCapabilitiesCommandBuilder.PLATFORM_FIRMWARE)
                .addResponseListener(r -> System.out.println(r.getName()))
                .build()
                .andThen(new DemoActivateCommandBuilder().addResponseListener(r -> System.out.println(r.alreadyActivated())).build())
                .andThen(new DemoActivateCommandBuilder().addResponseListener(r -> System.out.println(r.alreadyActivated())).build()));
        ZcodeCommandResponseSystem zcodeOutAddr = new ZcodeCommandResponseQueue(
                zcodeOut.getResponseAddressingSystem().getAddressConnection(ZcodeAddress.from(0x50, 0x0, 0x1)));
        zcodeOutAddr.send(new DemoCapabilitiesCommandBuilder()
                .setVersionType(DemoCapabilitiesCommandBuilder.PLATFORM_FIRMWARE)
                .addResponseListener(r -> System.out.println(r.getName()))
                .build());
    }
}
