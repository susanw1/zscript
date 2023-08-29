package net.zscript.javaclient.connection;

import static net.zscript.model.modules.base.CoreModule.CapabilitiesCommandBuilder.VersionType.PlatformFirmware;

import net.zscript.model.modules.base.CoreModule;

public class Main2 {
    public static void main(String[] args) {
        CommandResponseSystem zscriptOut = new CommandResponseQueue(new LocalZscriptConnection());

        zscriptOut.send(CoreModule.capabilities()
                .versionType(PlatformFirmware)
                .addResponseListener(r -> System.out.println(r.getIdent()))
                .build()
                .andThen(CoreModule.activate()
                        .addResponseListener(r -> System.out.println(r.isPreviousActivationState()))
                        .build())
                .andThen(CoreModule.activate()
                        .addResponseListener(r -> System.out.println(r.isPreviousActivationState()))
                        .build()));

        ZscriptAddress        address           = ZscriptAddress.from(0x50, 0x0, 0x1);
        ZscriptConnection     addressConnection = zscriptOut.getResponseAddressingSystem().getAddressConnection(address);
        CommandResponseSystem zscriptOutAddr    = new CommandResponseQueue(addressConnection);

        zscriptOutAddr.send(CoreModule.capabilities()
                .versionType(PlatformFirmware)
                .addResponseListener(r -> System.out.println(r.getIdent()))
                .build());
    }
}
