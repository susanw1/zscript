package org.zcode.javaclient.responseParser;

import java.util.HashMap;
import java.util.Map;
import java.util.function.Consumer;

public class ZcodeResponseAddressingSystem {
    private final ZcodeCommandResponseSystem          parent;
    private final Map<ZcodeAddress, Consumer<byte[]>> addressResp = new HashMap<>();

    public ZcodeResponseAddressingSystem(ZcodeCommandResponseSystem parent) {
        this.parent = parent;
    }

    public ZcodeConnection getAddressConnection(int[] addressSegments) {
        return new ZcodeConnection() {

            @Override
            public void send(byte[] data) {
                parent.send(addressSegments, data);
            }

            @Override
            public void onReceive(Consumer<byte[]> handler) {
                addressResp.put(new ZcodeAddress(addressSegments), handler);
            }
        };
    }

    public void response(int[] addr, byte[] received) {
        addressResp.get(new ZcodeAddress(addr)).accept(received);
    }

}
