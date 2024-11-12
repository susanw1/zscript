package net.zscript.javaclient.nodes;

import java.util.HashMap;
import java.util.Map;
import java.util.function.Consumer;

import net.zscript.javaclient.addressing.AddressedCommand;
import net.zscript.javaclient.addressing.AddressedResponse;
import net.zscript.javaclient.addressing.ZscriptAddress;
import net.zscript.javaclient.threading.ZscriptWorkerThread;

/**
 * Maintains the mapping of addresses to connections for a node in the device tree. The device node must be known at create-time, and the addresses are attached (or detached)
 * thereafter.
 */
class AddressingSystem {
    private final Map<ZscriptAddress, AddressingConnection> connections = new HashMap<>();

    private final ZscriptBasicNode node;

    AddressingSystem(ZscriptBasicNode node) {
        this.node = node;
    }

    private void sendAddressed(AddressedCommand cmd) {
        node.getParentConnection().send(cmd);
    }

    /**
     * Pass this response back to child connection using the {@link AddressedResponse} object's address info.
     *
     * @param addressedResponse the response to pass on
     * @return true if suitable connection found, false otherwise (response has not been passed on)
     */
    public boolean response(AddressedResponse addressedResponse) {
        AddressingConnection childConnection = connections.get(addressedResponse.getAddressSection());
        if (childConnection == null) {
            return false;
        }
        childConnection.response(addressedResponse.getChild());
        return true;
    }

    public Connection attach(ZscriptAddress address) {
        AddressingConnection connection = new AddressingConnection(address);
        connections.put(address, connection);
        return connection;
    }

    public Connection detach(ZscriptAddress address) {
        return connections.remove(address);
    }

    class AddressingConnection implements Connection {
        private final ZscriptAddress              address;
        private       Consumer<AddressedResponse> respConsumer = null;

        public AddressingConnection(ZscriptAddress address) {
            this.address = address;
        }

        @Override
        public void send(AddressedCommand cmd) {
            cmd.addAddressLayer(address);
            sendAddressed(cmd);
        }

        @Override
        public void onReceive(Consumer<AddressedResponse> responseHandler) {
            respConsumer = responseHandler;
        }

        public void response(AddressedResponse resp) {
            if (respConsumer != null) {
                respConsumer.accept(resp);
            }
        }

        @Override
        public void notifyResponseMatched(AddressedCommand foundCommand) {
            node.responseReceived(foundCommand);
        }

        @Override
        public ZscriptWorkerThread getAssociatedThread() {
            return node.getParentConnection().getAssociatedThread();
        }
    }
}
