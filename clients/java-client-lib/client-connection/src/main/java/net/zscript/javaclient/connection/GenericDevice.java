package net.zscript.javaclient.connection;

import java.io.IOException;
import java.util.function.Consumer;

import net.zscript.javaclient.commandbuilder.CommandSequence;

/**
 * Defines an entity which can take a zscript message in low-level form (byte[]), with or without an address, or high level form of a SyntaxTree node (CommandSeqElement)
 */
public interface GenericDevice {
    void send(CommandSequence sequence) throws IOException;

    void send(ZscriptAddress addr, byte[] data) throws IOException;

    void send(byte[] zscript, Consumer<byte[]> callback) throws IOException;

    ResponseAddressingSystem getResponseAddressingSystem();
}
