package net.zscript.javaclient.connection;

import java.io.IOException;
import java.util.function.Consumer;

import net.zscript.javaclient.commandbuilder.CommandSequenceNode;

/**
 * Defines a device which can take a zscript message in low-level form (byte[]), with or without an address, or high level form of a SyntaxTree node (CommandSeqElement)
 */
public interface DeviceNode {
    /**
     * High-level interface that sends the supplied Zscript command-sequence to this device (no address prefix). The sequence embeds its own callbacks for response handling.
     *
     * @param sequence
     * @throws IOException
     */
    void send(CommandSequenceNode sequence) throws IOException;

    /**
     * Sends the supplied data to an immediate child device of this device.
     *
     * @param addr                   the address <i>relative to this device</i> to send the supplied commandSequence
     * @param zscriptCommandSequence the encoded command-sequence bytes to send to the specified address
     * @throws IOException
     */
    void send(ZscriptAddress addr, byte[] zscriptCommandSequence) throws IOException;

    /**
     * Sends the supplied zscript sequence to this device, and notify the supplied handler of the response.
     *
     * @param zscriptCommandSequence
     * @param callback
     * @throws IOException
     */
    void send(byte[] zscriptCommandSequence, Consumer<byte[]> callback) throws IOException;

    RemoteConnectors getRemoteConnectors();
}
