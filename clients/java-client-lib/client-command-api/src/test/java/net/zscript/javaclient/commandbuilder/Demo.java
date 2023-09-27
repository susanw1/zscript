package net.zscript.javaclient.commandbuilder;

import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;

import net.zscript.javaclient.commandbuilder.DemoActivateCommand.DemoActivateCommandBuilder;

public class Demo {
    public static void main(String[] args) {
        byte[] data = new DemoCapabilitiesCommandBuilder()
                .setVersionType(DemoCapabilitiesCommandBuilder.PLATFORM_FIRMWARE)
                .addResponseListener(r -> System.out.println("Executed: " + r.getName()))
                .build()
                .onFail(new DemoActivateCommandBuilder().build())
                .compile();
        String s = StandardCharsets.UTF_8.decode(ByteBuffer.wrap(data)).toString();
        System.out.println(s);
    }
}
