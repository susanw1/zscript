package net.zscript.javaclient.connection;

import static java.util.Optional.ofNullable;

import java.io.IOException;
import java.util.HashMap;
import java.util.Map;
import java.util.function.Consumer;

public class ResponseAddressingSystem {
    private final GenericDevice parent;

    private final Map<ZscriptAddress, Consumer<byte[]>>  addressResponseHandlers = new HashMap<>();
    private final Map<ZscriptAddress, ZscriptConnection> addressConnection       = new HashMap<>();

    public ResponseAddressingSystem(GenericDevice parent) {
        this.parent = parent;
    }

    public ZscriptConnection getAddressConnection(ZscriptAddress address) {
        return addressConnection.computeIfAbsent(address, a -> new ZscriptConnection() {
            @Override
            public void send(byte[] data) throws IOException {
                parent.send(address, data);
            }

            @Override
            public void onReceive(Consumer<byte[]> handler) {
                addressResponseHandlers.put(a, handler);
            }

            @Override
            public void close() {
                addressResponseHandlers.remove(a);
                addressConnection.remove(a);
            }
        });
    }

    public void response(ZscriptAddress address, byte[] received) {
        ofNullable(addressResponseHandlers.get(address))
                .ifPresent(h -> h.accept(received));
    }
}
