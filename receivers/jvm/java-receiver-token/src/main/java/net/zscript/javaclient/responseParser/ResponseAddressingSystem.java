package net.zscript.javaclient.responseParser;

import java.util.HashMap;
import java.util.Map;
import java.util.function.Consumer;

public class ResponseAddressingSystem {
    private final CommandResponseSystem             parent;
    private final Map<ZscriptAddress, Consumer<byte[]>>  addressResp       = new HashMap<>();
    private final Map<ZscriptAddress, ZscriptConnection> addressConnection = new HashMap<>();

    public ResponseAddressingSystem(CommandResponseSystem parent) {
        this.parent = parent;
    }

    public ZscriptConnection getAddressConnection(ZscriptAddress addr) {
        return addressConnection.computeIfAbsent(addr, a -> new ZscriptConnection() {
            @Override
            public void send(byte[] data) {
                parent.send(addr, data);
            }

            @Override
            public void onReceive(Consumer<byte[]> handler) {
                addressResp.put(a, handler);
            }
        });
    }

    public void response(int[] addr, byte[] received) {
        addressResp.get(new ZscriptAddress(addr)).accept(received);
    }

}
