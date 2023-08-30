package net.zscript.javaclient.connection;

import java.util.HashMap;
import java.util.Map;
import java.util.function.Consumer;

public class ResponseAddressingSystem {
    private final DeviceCommunications parent;

    private final Map<ZscriptAddress, Consumer<byte[]>>  addressResp       = new HashMap<>();
    private final Map<ZscriptAddress, ZscriptConnection> addressConnection = new HashMap<>();

    public ResponseAddressingSystem(DeviceCommunications parent) {
        this.parent = parent;
    }

    public ZscriptConnection getAddressConnection(ZscriptAddress address) {
        return addressConnection.computeIfAbsent(address, a -> new ZscriptConnection() {
            @Override
            public void send(byte[] data) {
                parent.send(address, data);
            }

            @Override
            public void onReceive(Consumer<byte[]> handler) {
                addressResp.put(a, handler);
            }
        });
    }

    public void response(ZscriptAddress address, byte[] received) {
        addressResp.get(address).accept(received);
    }
}
