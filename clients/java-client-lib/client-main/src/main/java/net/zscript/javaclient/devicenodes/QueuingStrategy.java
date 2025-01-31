package net.zscript.javaclient.devicenodes;

import net.zscript.javaclient.commandpaths.CommandExecutionPath;
import net.zscript.javaclient.sequence.CommandSequence;

public interface QueuingStrategy {
    /**
     * Performs the necessary strategic actions when buffer space may have been freed, allowing more messages to be fed towards the target.
     */
    void bufferSpaceFreed();

    void forward(AddressedCommand seq);

    void send(CommandSequence seq);

    void send(CommandExecutionPath seq);

    void setBuffer(ConnectionBuffer buffer);
}
