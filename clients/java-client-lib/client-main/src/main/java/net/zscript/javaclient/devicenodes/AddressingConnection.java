package net.zscript.javaclient.devicenodes;

import java.util.function.Consumer;

import net.zscript.javaclient.addressing.AddressedCommand;
import net.zscript.javaclient.addressing.AddressedResponse;
import net.zscript.javaclient.addressing.ZscriptAddress;
import net.zscript.javaclient.threading.ZscriptWorkerThread;

/**
 * A connection representing a join between one node and its parent (as opposed to a connection hooking a root node to an actual means of interfacing with hardware - see
 * {@link DirectConnection}).
 */
class AddressingConnection implements Connection {
    private final ZscriptAddress          address;
    private final Connection              parentConnection;
    private final ResponseMatchedListener responseMatchedListener;

    private Consumer<AddressedResponse> responseHandler = null;

    public AddressingConnection(ZscriptAddress address, Connection parentConnection, ResponseMatchedListener responseMatchedListener) {
        this.address = address;
        this.parentConnection = parentConnection;
        this.responseMatchedListener = responseMatchedListener;
    }

    @Override
    public void send(AddressedCommand cmd) {
        cmd.addAddressLayer(address);
        parentConnection.send(cmd);
    }

    @Override
    public void onReceive(Consumer<AddressedResponse> responseHandler) {
        this.responseHandler = responseHandler;
    }

    public void response(AddressedResponse resp) {
        if (responseHandler != null) {
            responseHandler.accept(resp);
        }
    }

    @Override
    public void onResponseMatched(AddressedCommand foundCommand) {
        responseMatchedListener.onResponseMatched(foundCommand);
        parentConnection.onResponseMatched(foundCommand);
    }

    @Override
    public ZscriptWorkerThread getAssociatedThread() {
        return parentConnection.getAssociatedThread();
    }
}
