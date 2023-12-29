package net.zscript.javaclient.nodes;

import net.zscript.javaclient.addressing.AddressedCommand;
import net.zscript.javaclient.commandPaths.CommandExecutionPath;
import net.zscript.javaclient.sequence.CommandSequence;

public interface QueuingStrategy {
    void mayHaveSpace();

    void send(CommandSequence seq);

    void send(CommandExecutionPath seq);

    void send(AddressedCommand seq);

    void setBuffer(ConnectionBuffer buffer);
}
