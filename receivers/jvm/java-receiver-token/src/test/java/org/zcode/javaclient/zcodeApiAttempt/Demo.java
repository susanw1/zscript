package org.zcode.javaclient.zcodeApiAttempt;

import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;

public class Demo {
    public static void main(String[] args) {
        byte[] data = new DemoCapabilitiesCommandBuilder()
                .setVersionType(DemoCapabilitiesCommandBuilder.PLATFORM_FIRMWARE)
                .addResponseListener(r -> System.out.println(r.getName()))
                .build()
                .onFail(new DemoActivateCommandBuilder().build())
                .compile(true);
        String s    = StandardCharsets.UTF_8.decode(ByteBuffer.wrap(data)).toString();
        System.out.println(s);
    }
}
