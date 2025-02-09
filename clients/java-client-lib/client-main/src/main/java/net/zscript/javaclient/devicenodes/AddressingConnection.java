package net.zscript.javaclient.devicenodes;

import java.util.function.Consumer;

import net.zscript.javaclient.commandpaths.ZscriptAddress;

/**
 * A connection representing a join between one node and its parent (as opposed to a connection hooking a root node to an actual means of interfacing with hardware - see
 * {@link DirectConnection}).
 */
class AddressingConnection implements Connection {
    private final ZscriptAddress        address;
    private final Connection            parentConnection;
    private final ResponseMatchListener responseMatchListener;

    private Consumer<AddressedResponse> responseHandler = null;

    public AddressingConnection(ZscriptAddress address, Connection parentConnection, ResponseMatchListener responseMatchListener) {
        this.address = address;
        this.parentConnection = parentConnection;
        this.responseMatchListener = responseMatchListener;
    }

    @Override
    public void send(AddressedCommand cmd) {
        cmd.prefixAddressLayer(address);
        parentConnection.send(cmd);
    }

    @Override
    public void onReceive(Consumer<AddressedResponse> responseHandler) {
        this.responseHandler = responseHandler;
    }

    public void responseToChild(AddressedResponse resp) {
        if (responseHandler != null) {
            responseHandler.accept(resp);
        }
    }

    @Override
    public void responseHasMatched(AddressedCommand foundCommand) {
        responseMatchListener.responseHasMatched(foundCommand);
        parentConnection.responseHasMatched(foundCommand);
    }

    @Override
    public ZscriptWorkerThread getAssociatedThread() {
        return parentConnection.getAssociatedThread();
    }
}
