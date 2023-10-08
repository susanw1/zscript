package net.zscript.javaclient.nodes;

import java.util.ArrayDeque;
import java.util.Deque;
import java.util.Queue;
import java.util.function.Consumer;

import net.zscript.javaclient.addressing.AddressedResponse;
import net.zscript.javaclient.addressing.ZscriptAddress;
import net.zscript.javaclient.sequence.CommandSequence;
import net.zscript.javaclient.sequence.ResponseSequence;

public class ZscriptNode {
    private final AddressingSystem addressingSystem;

    private Connection                  parentConnection;
    private       Consumer<AddressedResponse> unknownResponseHandler;


    public ZscriptNode(AddressingSystem addressingSystem, Connection parentConnection) {
        this.addressingSystem = addressingSystem;
        this.parentConnection = parentConnection;
        parentConnection.onReceive(r -> {
            if (r.hasAddress()) {
                if (!addressingSystem.response(r)) {
                    unknownResponseHandler.accept(r);
                }
            } else {
                response(r.getContent());
            }
        });
    }

    public Connection attach(ZscriptAddress address) {
        return addressingSystem.attach(address);
    }

    public Connection detach(ZscriptAddress address) {
        return addressingSystem.detach(address);
    }

    public void send(CommandSequence sequence) {

    }

    private void response(ResponseSequence resp) {

    }

    public Connection getParentConnection() {
        return parentConnection;
    }

}
