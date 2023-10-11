package net.zscript.javaclient.commandPaths;

import java.util.ArrayDeque;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Deque;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.ListIterator;
import java.util.Map;
import java.util.Optional;
import java.util.Set;

import static net.zscript.javareceiver.tokenizer.TokenBuffer.TokenReader.ReadToken;

import net.zscript.javaclient.commandPrinting.CommandGrapher;
import net.zscript.javaclient.commandbuilder.AndSequenceNode;
import net.zscript.javaclient.commandbuilder.CommandSequenceNode;
import net.zscript.javaclient.commandbuilder.OrSequenceNode;
import net.zscript.javaclient.commandbuilder.ZscriptByteString;
import net.zscript.javaclient.commandbuilder.ZscriptCommandNode;
import net.zscript.javareceiver.tokenizer.TokenBufferIterator;
import net.zscript.javareceiver.tokenizer.Tokenizer;
import net.zscript.model.ZscriptModel;
import net.zscript.model.components.Zchars;
import net.zscript.util.ByteString;
import net.zscript.util.OptIterator;

public class CommandExecutionPath implements Iterable<Command> {

    static class CommandBuilder {
        ReadToken start = null;

        CommandBuilder onSuccess           = null;
        int            successBracketCount = 0;
        CommandBuilder onFail              = null;
        int            failBracketCount    = 0;

        public void setOnSuccess(CommandBuilder onSuccess) {
            this.onSuccess = onSuccess;
        }

        public void setOnFail(CommandBuilder onFail) {
            this.onFail = onFail;
        }

        public ReadToken getStart() {
            return start;
        }

        public void setStart(ReadToken token) {
            this.start = token;
        }

        private Command.CommandEndLink generateLinks(Map<CommandBuilder, Command> commands) {
            return new Command.CommandEndLink(commands.get(onSuccess), commands.get(onFail));
        }

        public Command generateCommand(Map<CommandBuilder, Command> otherCommands) {
            return new Command(generateLinks(otherCommands), ZscriptFieldSet.fromTokens(start));
        }
    }

    private static List<CommandBuilder> createLinkedPaths(ReadToken start) {
        List<CommandBuilder> needSuccessPath = new ArrayList<>();
        List<CommandBuilder> needFailPath    = new ArrayList<>();
        List<CommandBuilder> builders        = new ArrayList<>();

        CommandBuilder last = new CommandBuilder();
        builders.add(last);
        if (start == null) {
            return builders;
        }

        TokenBufferIterator iterator = start.getNextTokens();
        for (Optional<ReadToken> opt = iterator.next(); opt.isPresent(); opt = iterator.next()) {
            ReadToken token = opt.get();
            if (last.getStart() == null) {
                last.setStart(token);
            }
            if (token.isMarker()) {
                CommandBuilder next = new CommandBuilder();
                builders.add(next);
                if (token.getKey() == Tokenizer.CMD_END_ANDTHEN) {
                    last.setOnSuccess(next);
                    needFailPath.add(last);
                } else if (token.getKey() == Tokenizer.CMD_END_ORELSE) {
                    needSuccessPath.add(last);
                    last.setOnFail(next);
                    for (Iterator<CommandBuilder> iter = needFailPath.iterator(); iter.hasNext(); ) {
                        CommandBuilder b = iter.next();
                        if (b.failBracketCount == 0) {
                            b.setOnFail(next);
                            iter.remove();
                        }
                    }
                } else if (token.getKey() == Tokenizer.CMD_END_OPEN_PAREN) {
                    last.setOnSuccess(next);
                    needFailPath.add(last);
                    for (CommandBuilder b : needFailPath) {
                        b.failBracketCount++;
                    }
                    for (CommandBuilder b : needSuccessPath) {
                        b.successBracketCount++;
                    }
                } else if (token.getKey() == Tokenizer.CMD_END_CLOSE_PAREN) {
                    last.setOnSuccess(next);
                    needFailPath.add(last);
                    for (CommandBuilder b : needFailPath) {
                        if (b.failBracketCount > 0) {
                            b.failBracketCount--;
                        }
                    }
                    for (Iterator<CommandBuilder> iter = needSuccessPath.iterator(); iter.hasNext(); ) {
                        CommandBuilder b = iter.next();
                        if (b.successBracketCount == 0) {
                            b.setOnSuccess(next);
                            iter.remove();
                        } else {
                            b.successBracketCount--;
                        }
                    }
                } else if (token.isSequenceEndMarker()) {
                    break;
                } else {
                    throw new IllegalStateException("Unknown separator: " + Integer.toHexString(Byte.toUnsignedInt(token.getKey())));
                }
                last = next;
            }
        }
        return builders;
    }

    public static CommandExecutionPath parse(ReadToken start) {
        return parse(ZscriptModel.standardModel(), start);
    }

    public static CommandExecutionPath parse(ZscriptModel model, ReadToken start) {
        List<CommandBuilder> builders = createLinkedPaths(start);

        Map<CommandBuilder, Command> commands = new HashMap<>();
        for (ListIterator<CommandBuilder> iter = builders.listIterator(builders.size()); iter.hasPrevious(); ) {
            CommandBuilder b = iter.previous();
            if (b.start != null) {
                commands.put(b, b.generateCommand(commands));
            }
        }
        return new CommandExecutionPath(model, commands.get(builders.get(0)));
    }

    public static class CommandNodeToExecutionPath {
        private final CommandExecutionPath             path;
        private final Map<Command, ZscriptCommandNode> commandMap;

        public CommandNodeToExecutionPath(CommandExecutionPath path, Map<Command, ZscriptCommandNode> commandMap) {
            this.path = path;
            this.commandMap = commandMap;
        }

        public CommandExecutionPath getPath() {
            return path;
        }

        public Map<Command, ZscriptCommandNode> getCommandMap() {
            return commandMap;
        }
    }

    public static CommandNodeToExecutionPath convert(CommandSequenceNode cmdSeq) {
        return convert(ZscriptModel.standardModel(), cmdSeq);
    }

    public static CommandNodeToExecutionPath convert(ZscriptModel model, CommandSequenceNode cmdSeq) {
        class Layer {
            Command success;
            Command failure;
            boolean onSuccessHasOpenParen;
            boolean onSuccessHasCloseParen;

        }
        Map<Command, ZscriptCommandNode> commandMap = new HashMap<>();

        Deque<ListIterator<CommandSequenceNode>> stack = new ArrayDeque<>();
        Deque<CommandSequenceNode>               nodes = new ArrayDeque<>();

        Deque<Layer> destinations = new ArrayDeque<>();
        stack.push(List.of(cmdSeq).listIterator(1));

        Layer first = new Layer();
        first.success = null;
        first.failure = null;
        first.onSuccessHasOpenParen = false;
        destinations.push(first);

        while (true) {
            if (stack.peek().hasPrevious()) {
                Layer               layer = destinations.peek();
                CommandSequenceNode next  = stack.peek().previous();
                if (next instanceof ZscriptCommandNode) {
                    Command cmd = new Command(new Command.CommandEndLink(layer.success, layer.failure),
                            ZscriptFieldSet.from((ZscriptCommandNode) next));
                    commandMap.put(cmd, (ZscriptCommandNode) next);
                    if (nodes.peek() instanceof OrSequenceNode && stack.peek().hasPrevious()) {
                        layer.failure = cmd;
                    } else {
                        layer.success = cmd;
                        layer.onSuccessHasOpenParen = false;
                        layer.onSuccessHasCloseParen = false;
                    }
                } else if (next instanceof OrSequenceNode) {
                    Layer nextLayer = new Layer();
                    if (nodes.peek() instanceof AndSequenceNode) {
                        if (layer.onSuccessHasCloseParen) {
                            layer.success = new Command(new Command.CommandEndLink(layer.success, layer.failure),
                                    ZscriptFieldSet.blank());
                        }
                        layer.onSuccessHasCloseParen = true;
                    }
                    nextLayer.onSuccessHasCloseParen = true;
                    nextLayer.failure = layer.failure;
                    nextLayer.success = layer.success;
                    nextLayer.onSuccessHasOpenParen = false;
                    stack.push(next.getChildren().listIterator(next.getChildren().size()));
                    nodes.push(next);
                    destinations.push(nextLayer);
                } else if (next instanceof AndSequenceNode) {
                    Layer nextLayer = new Layer();
                    nextLayer.failure = layer.failure;
                    nextLayer.success = layer.success;
                    nextLayer.onSuccessHasOpenParen = false;
                    nextLayer.onSuccessHasCloseParen = layer.onSuccessHasCloseParen;
                    stack.push(next.getChildren().listIterator(next.getChildren().size()));
                    nodes.push(next);
                    destinations.push(nextLayer);
                } else {
                    throw new IllegalStateException("Unknown type: " + next);
                }
            } else {
                stack.pop();
                if (nodes.isEmpty()) {
                    break;
                }
                CommandSequenceNode prev      = nodes.pop();
                Layer               prevLayer = destinations.pop();
                Layer               layer     = destinations.peek();
                layer.onSuccessHasOpenParen = prevLayer.onSuccessHasOpenParen;
                if (nodes.peek() instanceof AndSequenceNode) {
                    layer.success = prevLayer.success;
                    if (prev instanceof OrSequenceNode) {
                        if (prevLayer.onSuccessHasOpenParen) {
                            layer.success = new Command(
                                    new Command.CommandEndLink(prevLayer.success, prevLayer.failure),
                                    ZscriptFieldSet.blank());
                        }
                        layer.onSuccessHasOpenParen = true;
                    }
                } else if (stack.peek().hasPrevious()) {
                    layer.failure = prevLayer.success;
                } else {
                    layer.success = prevLayer.success;
                }
            }
        }
        return new CommandNodeToExecutionPath(new CommandExecutionPath(model, destinations.peek().success), commandMap);
    }

    public static CommandExecutionPath blank() {
        return blank(ZscriptModel.standardModel());
    }

    public static CommandExecutionPath blank(ZscriptModel model) {
        return new CommandExecutionPath(model, null);
    }

    private final ZscriptModel model;
    private final Command      firstCommand;

    private CommandExecutionPath(ZscriptModel model, Command firstCommand) {
        this.model = model;
        this.firstCommand = firstCommand;
    }

    public ByteString toSequence() {
        ZscriptByteString.ZscriptByteStringBuilder out = ZscriptByteString.builder();
        toSequence(out);
        return out.build();
    }

    public void toSequence(ZscriptByteString.ZscriptByteStringBuilder out) {
        Deque<Command> openedTrees  = new ArrayDeque<>();
        Deque<Command> workingTrees = new ArrayDeque<>();
        if (firstCommand != null) {
            workingTrees.push(firstCommand);
        }
        boolean needsAnd  = false;
        boolean needsOpen = false;
        while (!workingTrees.isEmpty()) {
            Command                current = workingTrees.peek();
            Command.CommandEndLink links   = current.getEndLink();

            Command latestOpenTree = openedTrees.peek();
            if (latestOpenTree != null && links.getOnFail() != latestOpenTree) {
                // if there are opened trees, and this command doesn't add to the top open tree,
                //   check if it closes the top open tree
                //   which we can do by iterating forward to see if the top open tree re-merges here
                // this operation makes our graph drawing O(n^2)
                Command tmp        = latestOpenTree;
                boolean mergesHere = false;
                while (tmp != null) {
                    if (tmp == current) {
                        mergesHere = true;
                        break;
                    }
                    tmp = tmp.getEndLink().getOnSuccess();
                }
                if (mergesHere) {
                    openedTrees.pop();
                    workingTrees.push(latestOpenTree);
                    out.appendByte(Zchars.Z_ORELSE);
                    needsOpen = false;
                    needsAnd = false;
                    // if it closes the top open tree, process that tree first
                    continue;
                }
            }
            if (links.getOnFail() != null && links.getOnFail() != latestOpenTree) {
                // the command opens a new open tree
                if (needsOpen) {
                    out.appendByte(Zchars.Z_OPENPAREN);
                }
                openedTrees.push(links.getOnFail());
                latestOpenTree = links.getOnFail();
            } else if (needsAnd) {
                out.appendByte(Zchars.Z_ANDTHEN);
            }

            workingTrees.pop().toBytes(out); // only put elements into the list when we're done processing them
            needsAnd = false;
            needsOpen = true;
            if (links.getOnSuccess() != null) {
                if (workingTrees.isEmpty() || links.getOnSuccess() != workingTrees.peek()) {
                    // check we're not on a close parent, dropping out of failure
                    workingTrees.push(links.getOnSuccess());
                    needsAnd = true;
                } else {
                    out.appendByte(Zchars.Z_CLOSEPAREN);
                }
            } else if (latestOpenTree != null) {
                openedTrees.pop();
                workingTrees.push(latestOpenTree);
                out.appendByte(Zchars.Z_ORELSE);
                needsOpen = false;
            }
        }
    }

    public List<MatchedCommandResponse> compareResponses(ResponseExecutionPath resps) {
        List<MatchedCommandResponse> cmds        = new ArrayList<>();
        Command                      current     = firstCommand;
        Response                     currentResp = resps.getFirstResponse();
        while (currentResp != null) {
            if (current == null) {
                throw new IllegalArgumentException("Response doesn't match command seq");
            } else {
                cmds.add(new MatchedCommandResponse(current, currentResp));
            }
            if (currentResp.wasSuccess()) {
                current = current.getEndLink().getOnSuccess();
            } else {
                current = current.getEndLink().getOnFail();
            }
            currentResp = currentResp.getNext();
        }
        return cmds;
    }

    public Iterator<Command> iterator() {
        if (firstCommand == null) {
            return Collections.emptyListIterator();
        }
        return new OptIterator<Command>() {
            final Set<Command> visited = new HashSet<>();
            Iterator<Command> toVisit = Set.of(firstCommand).iterator();
            Set<Command> next = new HashSet<>();

            @Override
            public Optional<Command> next() {
                while (true) {
                    while (toVisit.hasNext()) {
                        Command c = toVisit.next();
                        if (!visited.contains(c)) {
                            visited.add(c);
                            Command s = c.getEndLink().getOnSuccess();
                            if (!visited.contains(s)) {
                                next.add(s);
                            }
                            Command f = c.getEndLink().getOnFail();
                            if (!visited.contains(f)) {
                                next.add(f);
                            }
                            return Optional.of(c);
                        }
                    }
                    if (next.isEmpty()) {
                        return Optional.empty();
                    }
                    toVisit = next.iterator();
                    next = new HashSet<>();
                }
            }
        }.stream().iterator();
    }

    public int getBufferLength() {
        int length = 0;
        for (Command c : this) {
            length += c.getBufferLength() + 1;
        }
        return length;
    }

    public Command getFirstCommand() {
        return firstCommand;
    }

    public ZscriptModel getModel() {
        return model;
    }
}