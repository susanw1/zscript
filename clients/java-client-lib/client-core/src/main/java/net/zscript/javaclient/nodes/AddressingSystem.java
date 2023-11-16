package net.zscript.javaclient.nodes;

import java.util.HashMap;
import java.util.Map;
import java.util.function.Consumer;

import net.zscript.javaclient.addressing.AddressedCommand;
import net.zscript.javaclient.addressing.AddressedResponse;
import net.zscript.javaclient.addressing.ZscriptAddress;
import net.zscript.javaclient.threading.ZscriptWorkerThread;

class AddressingSystem {
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
        public void onReceive(Consumer<AddressedResponse> resp) {
            respConsumer = resp;
        }

        public void response(AddressedResponse resp) {
            if (respConsumer != null) {
                respConsumer.accept(resp);
            }
        }

        @Override
        public void responseReceived(AddressedCommand found) {
            node.responseReceived(found);
        }

        @Override
        public ZscriptWorkerThread getAssociatedThread() {
            return node.getParentConnection().getAssociatedThread();
        }
    }

    private final Map<ZscriptAddress, AddressingConnection> connections = new HashMap<>();

    private final ZscriptBasicNode node;

    AddressingSystem(ZscriptBasicNode node) {
        this.node = node;
    }

    private void sendAddressed(AddressedCommand cmd) {
        node.getParentConnection().send(cmd);
    }

    public boolean response(AddressedResponse addrResp) {
        AddressingConnection connection = connections.get(addrResp.getAddressSection());
        if (connection == null) {
            return false;
        }
        connection.response(addrResp.getChild());
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
}
