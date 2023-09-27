package net.zscript.javaclient.connection;

import static net.zscript.javaclient.connection.ZscriptAddressPath.path;

import java.util.List;

import net.zscript.javaclient.commandbuilder.CommandSequenceNode;

abstract class ExpressionSequence<S extends ExpressionSequence<S>> {
    /**
     * Addresses in top-down hierarchical order.
     */
    List<ZscriptAddress> addressPath;
    Integer              echoNumber;

    ExpressionSequence() {
    }

    public S setEchoNumber(int echoNumber) {
        this.echoNumber = echoNumber;
        return (S) this;
    }

    public S appendSubAddress(ZscriptAddress subAddress) {
        addressPath.add(subAddress);
        return (S) this;
    }

    public S prependSubAddress(ZscriptAddress subAddress) {
        addressPath.add(0, subAddress);
        return (S) this;
    }

    public ZscriptAddressPath getAddressPath() {
        return path(addressPath);
    }

    public abstract byte[] asZscriptBytes();
}
