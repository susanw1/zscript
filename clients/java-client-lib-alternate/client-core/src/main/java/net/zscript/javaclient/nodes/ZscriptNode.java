package net.zscript.javaclient.nodes;

import java.util.ArrayDeque;
import java.util.Deque;
import java.util.Queue;
import java.util.function.Consumer;

import net.zscript.javaclient.addressing.AddressedCommand;
import net.zscript.javaclient.addressing.AddressedResponse;
import net.zscript.javaclient.addressing.ZscriptAddress;
import net.zscript.javaclient.commandPaths.CommandExecutionPath;
import net.zscript.javaclient.sequence.CommandSequence;
import net.zscript.javaclient.sequence.ResponseSequence;

public class ZscriptNode {
    private final AddressingSystem addressingSystem;

    private final ConnectionBuffer connectionBuffer;
    private final Connection       parentConnection;

    private QueuingStrategy             strategy = new StandardQueuingStrategy();
    private Consumer<AddressedResponse> unknownResponseHandler;

    public ZscriptNode(AddressingSystem addressingSystem, Connection parentConnection, int bufferSize) {
        this.addressingSystem = addressingSystem;
        this.parentConnection = parentConnection;
        this.connectionBuffer = new ConnectionBuffer(parentConnection, bufferSize);
        parentConnection.onReceive(r -> {
            if (r.hasAddress()) {
                if (!addressingSystem.response(r)) {
                    unknownResponseHandler.accept(r);
                }
            } else {
                response(r);
            }
        });
    }

    public void setUnknownResponseHandler(Consumer<AddressedResponse> unknownResponseHandler) {
        this.unknownResponseHandler = unknownResponseHandler;
    }

    public void setStrategy(QueuingStrategy strategy) {
        this.strategy = strategy;
    }

    public Connection attach(ZscriptAddress address) {
        return addressingSystem.attach(address);
    }

    public Connection detach(ZscriptAddress address) {
        return addressingSystem.detach(address);
    }

    public void send(CommandSequence seq) {
        strategy.send(seq);
    }

    public void send(CommandExecutionPath path) {
        strategy.send(path);
    }

    public void send(AddressedCommand addr) {
        strategy.send(addr);
    }

    private void response(AddressedResponse resp) {
        if (resp.getContent().getResponseValue() != 0) {
            // TODO: Notification handlers
        }
        AddressedCommand found = connectionBuffer.match(resp.getContent());
        if (found == null) {
            unknownResponseHandler.accept(resp);
            return;
        }
        strategy.mayHaveSpace();
        parentConnection.responseReceived(found);
        // TODO: Response callbacks
    }

    public Connection getParentConnection() {
        return parentConnection;
    }

    public void responseReceived(AddressedCommand found) {
        if (connectionBuffer.responseReceived(found)) {
            strategy.mayHaveSpace();
        }
        parentConnection.responseReceived(found);
    }
}
