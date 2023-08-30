package net.zscript.javaclient.connection;

import java.util.function.Consumer;

import net.zscript.javaclient.commandbuilder.CommandSequence;

/**
 * Defines an entity which can take a zscript message in low-level form (byte[]), with or without an address, or high level form of a SyntaxTree node (CommandSeqElement)
 */
public interface GenericDevice {
    void send(CommandSequence sequence);

    void send(ZscriptAddress addr, byte[] data);

    void send(byte[] zscript, Consumer<byte[]> callback);

    ResponseAddressingSystem getResponseAddressingSystem();
}
