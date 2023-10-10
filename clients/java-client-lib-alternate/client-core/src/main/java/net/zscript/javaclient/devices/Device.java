package net.zscript.javaclient.devices;

import net.zscript.javaclient.commandPaths.CommandExecutionPath;
import net.zscript.javaclient.commandbuilder.CommandSequenceNode;
import net.zscript.javaclient.nodes.ZscriptNode;

public class Device {
    private final ZscriptNode node;

    public Device(ZscriptNode node) {
        this.node = node;
    }

    public void send(CommandSequenceNode cmdSeq) {
        CommandExecutionPath path = CommandExecutionPath.convert(cmdSeq);
    }
}
