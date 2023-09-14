package net.zscript.javaclient.connection;

import static java.util.Optional.ofNullable;

import java.io.IOException;
import java.util.HashMap;
import java.util.Map;
import java.util.function.Consumer;

/**
 * Represents the downstream connectors on a device (eg I2C, Serial etc). It converts the device-tree into addresses for the addressing system.
 */
public class RemoteConnectors {
    private final DeviceNode device;

    /** These Consumers consume the messages received from corresponding addressed devices */
    private final Map<ZscriptAddress, Consumer<byte[]>> addressResponseHandlers = new HashMap<>();

    /** These are the Connections used for communicating with the corresponding addressed devices */
    private final Map<ZscriptAddress, ZscriptConnection> addressConnection = new HashMap<>();

    public RemoteConnectors(DeviceNode device) {
        this.device = device;
    }

    /**
     * @param address
     * @return
     */
    public ZscriptConnection getAddressConnection(ZscriptAddress address) {
        return addressConnection.computeIfAbsent(address, a -> new ZscriptConnection() {
            @Override
            public void send(byte[] data) throws IOException {
                device.send(a, data);
            }

            @Override
            public void onReceive(Consumer<byte[]> responseHandler) {
                addressResponseHandlers.put(a, responseHandler);
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
