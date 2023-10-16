package net.zscript.javaclient.devices;

import java.util.function.Consumer;

import net.zscript.javaclient.commandPaths.CommandExecutionPath;
import net.zscript.javaclient.commandbuilder.commandnodes.CommandSequenceNode;
import net.zscript.javaclient.nodes.ZscriptNode;
import net.zscript.javaclient.sequence.CommandSequence;
import net.zscript.javareceiver.tokenizer.TokenExtendingBuffer;
import net.zscript.javareceiver.tokenizer.Tokenizer;
import net.zscript.model.ZscriptModel;

public class Device {
    private final ZscriptModel model;
    private final ZscriptNode  node;

    public Device(ZscriptModel model, ZscriptNode node) {
        this.model = model;
        this.node = node;
    }

    public void send(final CommandSequenceNode cmdSeq, final Consumer<ResponseSequenceCallback> callback) {
        CommandExecutionPath.CommandExecutionTask nodeToPath = CommandExecutionPath.convert(model, cmdSeq, callback);
        node.send(nodeToPath.getPath(), nodeToPath.getCallback());
    }

    public void send(final byte[] cmdSeq, final Consumer<byte[]> callback) {
        TokenExtendingBuffer buffer = new TokenExtendingBuffer();
        Tokenizer            tok    = new Tokenizer(buffer.getTokenWriter(), 2);
        for (byte b : cmdSeq) {
            tok.accept(b);
        }
        CommandSequence sequence = CommandSequence.parse(model, buffer.getTokenReader().getFirstReadToken(), false);
        if (sequence.hasEchoField() || sequence.hasLockField()) {
            node.send(sequence, r -> {
                callback.accept(r.toSequence().toByteArray());
            });
        } else {
            node.send(sequence.getExecutionPath(), r -> {
                callback.accept(r.toSequence().toByteArray());
            });
        }
    }
}
