package net.zscript.javaclient.connection;

import java.util.function.Consumer;

import net.zscript.javaclient.commandbuilder.CommandSeqElement;

public interface CommandResponseSystem {
    void send(CommandSeqElement seq);

    void send(ZscriptAddress addr, byte[] data);

    void send(byte[] zscript, Consumer<byte[]> callback);

    ResponseAddressingSystem getResponseAddressingSystem();
}