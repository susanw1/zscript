package net.zscript.javaclient.connection;

import static net.zscript.model.modules.base.CoreModule.CapabilitiesCommandBuilder.VersionType.PlatformFirmware;

import java.io.IOException;

import net.zscript.model.modules.base.CoreModule;

public class Main2 {
    public static void main(String[] args) throws IOException {
        DeviceNode zscriptOut = new CommandResponseQueue(new LocalZscriptConnection());

        zscriptOut.send(CoreModule.capabilities()
                .setVersionType(PlatformFirmware)
                .addResponseListener(r -> System.out.println(r.getIdent()))
                .build()
                .andThen(CoreModule.activate()
                        .addResponseListener(r -> System.out.println(r.isPreviousActivationState()))
                        .build())
                .andThen(CoreModule.activate()
                        .addResponseListener(r -> System.out.println(r.isPreviousActivationState()))
                        .build()));

        ZscriptAddress    address           = ZscriptAddress.from(0x50, 0x0, 0x1);
        ZscriptConnection addressConnection = zscriptOut.getRemoteConnectors().getAddressConnection(address);

        DeviceNode zscriptOutAddr = new CommandResponseQueue(addressConnection);

        //        zscriptOutAddr.send(CoreModule.capabilities()
        //                .versionType(PlatformFirmware)
        //                .addResponseListener(r -> System.out.println(r.getIdent()))
        //                .build());
    }
}
