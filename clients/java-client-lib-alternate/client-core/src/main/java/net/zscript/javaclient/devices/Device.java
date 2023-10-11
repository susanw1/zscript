package net.zscript.javaclient.devices;

import java.util.List;
import java.util.Map;
import java.util.function.Consumer;

import net.zscript.javaclient.commandPaths.Command;
import net.zscript.javaclient.commandPaths.CommandExecutionPath;
import net.zscript.javaclient.commandPaths.MatchedCommandResponse;
import net.zscript.javaclient.commandbuilder.CommandSequenceNode;
import net.zscript.javaclient.commandbuilder.ZscriptCommandNode;
import net.zscript.javaclient.nodes.ZscriptNode;
import net.zscript.javaclient.sequence.CommandSequence;
import net.zscript.javareceiver.tokenizer.TokenExtendingBuffer;
import net.zscript.javareceiver.tokenizer.Tokenizer;
import net.zscript.model.ZscriptModel;

public class Device {
    private final ZscriptModel model;
    private final ZscriptNode  node;

    public Device(ZscriptNode node) {
        this.node = node;
    }

    public void send(final CommandSequenceNode cmdSeq, final Consumer<ResponseSequenceCallback> callback) {
        CommandExecutionPath.CommandNodeToExecutionPath nodeToPath = CommandExecutionPath.convert(model, cmdSeq);

        CommandExecutionPath             path       = nodeToPath.getPath();
        Map<Command, ZscriptCommandNode> commandMap = nodeToPath.getCommandMap();

        node.send(path, resps -> {
            List<MatchedCommandResponse> matchedCRs = path.compareResponses(resps);
            for (MatchedCommandResponse cr : matchedCRs) {
                commandMap.get(cr.getCommand()).onResponse(cr.getResponse());
            }
            callback.accept();
        });
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
                callback.accept(r.toBytes());
            });
        } else {
            node.send(sequence.getExecutionPath(), r -> {
                callback.accept(r.toBytes());
            });
        }
    }
}
