package net.zscript.javaclient.connection;

import java.util.function.Consumer;

import net.zscript.javaclient.commandbuilder.CommandSeqElement;

/**
 * Defines an entity which can take a zscript message in low-level form (byte[]) or high level (
 */
public interface CommandResponseSystem {
    void send(CommandSeqElement seq);

    void send(ZscriptAddress addr, byte[] data);

    void send(byte[] zscript, Consumer<byte[]> callback);

    ResponseAddressingSystem getResponseAddressingSystem();
}
