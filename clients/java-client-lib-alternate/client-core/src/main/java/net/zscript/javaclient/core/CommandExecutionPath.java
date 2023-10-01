package net.zscript.javaclient.core;

import java.io.ByteArrayOutputStream;
import java.util.ArrayDeque;
import java.util.ArrayList;
import java.util.Deque;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.ListIterator;
import java.util.Map;
import java.util.Optional;

import net.zscript.javareceiver.tokenizer.TokenBuffer;
import net.zscript.javareceiver.tokenizer.TokenBufferIterator;
import net.zscript.javareceiver.tokenizer.Tokenizer;
import net.zscript.model.ZscriptModel;
import net.zscript.model.components.Zchars;

public class CommandExecutionPath {

    static class CommandBuilder {
        TokenBuffer.TokenReader.ReadToken start = null;

        CommandBuilder onSuccess           = null;
        int            successBracketCount = 0;
        List<Byte>     successSeparators   = new ArrayList<>();

        CommandBuilder onFail           = null;
        int            failBracketCount = 0;
        List<Byte>     failSeparators   = new ArrayList<>();

        public void setOnSuccess(CommandBuilder onSuccess) {
            this.onSuccess = onSuccess;
        }

        public void setOnFail(CommandBuilder onFail) {
            this.onFail = onFail;
        }

        public TokenBuffer.TokenReader.ReadToken getStart() {
            return start;
        }

        public void setStart(TokenBuffer.TokenReader.ReadToken token) {
            this.start = token;
        }

        private Command.CommandEndLink generateLinks(Map<CommandBuilder, Command> commands) {
            byte[] successSepArr = new byte[successSeparators.size()];

            int i = 0;
            for (byte b : successSeparators) {
                successSepArr[i++] = b;
            }
            byte[] failSepArr = new byte[failSeparators.size()];
            i = 0;
            for (byte b : failSeparators) {
                failSepArr[i++] = b;
            }
            return new Command.CommandEndLink(commands.get(onSuccess), successSepArr, commands.get(onFail), failSepArr);
        }

        public Command generateCommand(Map<CommandBuilder, Command> otherCommands) {
            return new Command(generateLinks(otherCommands), ZscriptFieldSet.fromTokens(start));
        }
    }

    private static List<CommandBuilder> createLinkedPaths(TokenBuffer.TokenReader.ReadToken start) {
        List<CommandBuilder> needSuccessPath = new ArrayList<>();
        List<CommandBuilder> needFailPath    = new ArrayList<>();
        List<CommandBuilder> builders        = new ArrayList<>();

        CommandBuilder last = new CommandBuilder();
        builders.add(last);

        TokenBufferIterator iterator = start.getNextTokens();
        for (Optional<TokenBuffer.TokenReader.ReadToken> opt = iterator.next(); opt.isPresent(); opt = iterator.next()) {
            TokenBuffer.TokenReader.ReadToken token = opt.get();
            if (last.getStart() == null) {
                last.setStart(token);
            }
            if (token.isMarker()) {
                CommandBuilder next = new CommandBuilder();
                builders.add(next);
                if (token.getKey() == Tokenizer.CMD_END_ANDTHEN) {
                    last.setOnSuccess(next);
                    last.successSeparators.add(Zchars.Z_ANDTHEN);
                    needFailPath.add(last);
                } else if (token.getKey() == Tokenizer.CMD_END_ORELSE) {
                    needSuccessPath.add(last);
                    last.setOnFail(next);
                    last.failSeparators.add(Zchars.Z_ORELSE);
                    for (Iterator<CommandBuilder> iter = needFailPath.iterator(); iter.hasNext(); ) {
                        CommandBuilder b = iter.next();
                        if (b.failBracketCount == 0) {
                            b.setOnFail(next);
                            b.failSeparators.add(Zchars.Z_ORELSE);
                            iter.remove();
                        }
                    }
                } else if (token.getKey() == Tokenizer.CMD_END_OPEN_PAREN) {
                    last.setOnSuccess(next);
                    needFailPath.add(last);
                    last.successSeparators.add(Zchars.Z_OPENPAREN);
                    for (CommandBuilder b : needFailPath) {
                        b.failBracketCount++;
                    }
                    for (CommandBuilder b : needSuccessPath) {
                        b.successBracketCount++;
                    }
                } else if (token.getKey() == Tokenizer.CMD_END_CLOSE_PAREN) {
                    last.setOnSuccess(next);
                    needFailPath.add(last);
                    last.successSeparators.add(Zchars.Z_CLOSEPAREN);
                    for (CommandBuilder b : needFailPath) {
                        if (b.failBracketCount > 0) {
                            b.failBracketCount--;
                        } else {
                            b.failSeparators.add(Zchars.Z_CLOSEPAREN);
                        }
                    }
                    for (Iterator<CommandBuilder> iter = needSuccessPath.iterator(); iter.hasNext(); ) {
                        CommandBuilder b = iter.next();
                        if (b.successBracketCount == 0) {
                            b.setOnSuccess(next);
                            b.successSeparators.add(Zchars.Z_CLOSEPAREN);
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

    public static CommandExecutionPath parse(TokenBuffer.TokenReader.ReadToken start) {
        return parse(ZscriptModel.standardModel(), start);
    }

    public static CommandExecutionPath parse(ZscriptModel model, TokenBuffer.TokenReader.ReadToken start) {
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

    private final ZscriptModel model;
    private final Command      firstCommand;

    private CommandExecutionPath(ZscriptModel model, Command firstCommand) {
        this.model = model;
        this.firstCommand = firstCommand;
    }

    public List<Command> compareResponses(ResponseExecutionPath resps) {
        List<Command> cmds        = new ArrayList<>();
        Command       current     = firstCommand;
        Response      currentResp = resps.getFirstResponse();
        while (currentResp != null) {
            if (current == null) {
                throw new IllegalArgumentException("Response doesn't match command seq");
            } else {
                cmds.add(current);
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

    public byte[] toSequence() {
        ByteArrayOutputStream out = new ByteArrayOutputStream();

        Command current = firstCommand;
        while (true) {
            current.toBytes(out);
            Command.CommandEndLink link = current.getEndLink();
            if (link.getOnSuccess() == null) {
                if (link.getOnFail() == null) {
                    break;
                } else {
                    out.write(link.getFailSeparators()[0]);
                    current = link.getOnFail();
                }
            } else if (link.getSuccessSeparators()[0] == Zchars.Z_CLOSEPAREN) {
                if (link.getOnFail() == null || link.getFailSeparators()[0] == Zchars.Z_CLOSEPAREN) {
                    out.write(Zchars.Z_CLOSEPAREN);
                    current = link.getOnSuccess();
                } else {
                    out.write(Zchars.Z_ORELSE);
                    current = link.getOnFail();
                }
            } else {
                out.write(link.getSuccessSeparators()[0]);
                current = link.getOnSuccess();
            }
        }
        return out.toByteArray();
    }

    public String graphPrint(CommandGrapher grapher, boolean skipImpossiblePaths) {
        return graphPrint(grapher, skipImpossiblePaths, List.of());
    }

    public String graphPrint(CommandGrapher grapher, boolean skipImpossiblePaths, List<Command> toHighlight) {
        CommandGrapher.CommandDepth        startDepth = new CommandGrapher.CommandDepth(0);
        CommandGrapher.CommandGraphElement start      = new CommandGrapher.CommandGraphElement(firstCommand, startDepth);

        Map<Command, CommandGrapher.CommandGraphElement> commands = new HashMap<>();

        Deque<CommandGrapher.CommandGraphElement> openedTrees  = new ArrayDeque<>();
        Deque<CommandGrapher.CommandGraphElement> workingTrees = new ArrayDeque<>();
        List<CommandGrapher.CommandGraphElement>  elements     = new ArrayList<>();

        commands.put(firstCommand, start);
        workingTrees.push(start);
        CommandGrapher.CommandDepth maxDepth = new CommandGrapher.CommandDepth(0);
        while (!workingTrees.isEmpty()) {
            CommandGrapher.CommandGraphElement current = workingTrees.peek();
            Command.CommandEndLink             links   = current.getCommand().getEndLink();

            CommandGrapher.CommandGraphElement latestOpenTree = openedTrees.peek();
            if (latestOpenTree != null && links.getOnFail() != latestOpenTree.getCommand()) {
                // if there are opened trees, and this command doesn't add to the top open tree,
                //   check if it closes the top open tree
                //   which we can do by iterating forward to see if the top open tree re-merges here
                // this operation makes our graph drawing O(n^2)
                Command tmp        = latestOpenTree.getCommand();
                boolean mergesHere = false;
                while (tmp != null) {
                    if (tmp == current.getCommand()) {
                        mergesHere = true;
                        break;
                    }
                    tmp = tmp.getEndLink().getOnSuccess();
                }
                if (mergesHere) {
                    openedTrees.pop();
                    workingTrees.push(latestOpenTree);
                    if (!openedTrees.isEmpty()) {
                        openedTrees.peek().getDepth().depthGreaterThan(latestOpenTree.getDepth());
                        maxDepth.depthGreaterThan(latestOpenTree.getDepth());
                    }
                    // if it closes the top open tree, process that tree first
                    continue;
                }
            }
            if (links.getOnFail() != null && (!skipImpossiblePaths || current.getCommand().canFail()) && (latestOpenTree == null
                    || links.getOnFail() != latestOpenTree.getCommand())) {
                // the command opens a new open tree
                CommandGrapher.CommandGraphElement opened = new CommandGrapher.CommandGraphElement(links.getOnFail(), new CommandGrapher.CommandDepth(current.getDepth()));
                maxDepth.depthGreaterThan(current.getDepth());
                commands.put(opened.getCommand(), opened);
                openedTrees.push(opened);
                latestOpenTree = opened;
            }

            elements.add(workingTrees.pop()); // only put elements into the list when we're done processing them
            if (links.getOnSuccess() != null && (!skipImpossiblePaths || current.getCommand().canSucceed())) {
                if (workingTrees.isEmpty() || links.getOnSuccess() != workingTrees.peek().getCommand()) {
                    // check we're not on a close parent, dropping out of failure
                    CommandGrapher.CommandGraphElement next = new CommandGrapher.CommandGraphElement(links.getOnSuccess(), current.getDepth());
                    commands.put(next.getCommand(), next);
                    workingTrees.push(next);
                }
            } else if (latestOpenTree != null) {
                openedTrees.pop();
                workingTrees.push(latestOpenTree);
                if (!openedTrees.isEmpty()) {
                    openedTrees.peek().getDepth().depthGreaterThan(latestOpenTree.getDepth());
                    maxDepth.depthGreaterThan(latestOpenTree.getDepth());
                }
            }
        }
        if (skipImpossiblePaths) {
            elements = skipEmpty(commands, elements);
            for (Iterator<Command> iter = toHighlight.iterator(); iter.hasNext(); ) {
                if (!elements.contains(commands.get(iter.next()))) {
                    iter.remove();
                }
            }
            //then trim depths which aren't used...
            int offset = 0;
            for (int i = 0; i < maxDepth.getDepth() + 1; i++) {
                boolean hasAtDepth = false;
                for (CommandGrapher.CommandGraphElement element : elements) {
                    if (element.getDepth().getDepth() == i) {
                        hasAtDepth = true;
                        element.getDepth().setDepth(i - offset);
                    }
                }
                if (!hasAtDepth) {
                    offset++;
                }
            }
        }
        return grapher.graph(commands, elements, maxDepth, toHighlight, skipImpossiblePaths);
    }

    private List<CommandGrapher.CommandGraphElement> skipEmpty(Map<Command, CommandGrapher.CommandGraphElement> commands,
            List<CommandGrapher.CommandGraphElement> elements) {
        List<CommandGrapher.CommandGraphElement> compactedElements = new ArrayList<>();
        for (CommandGrapher.CommandGraphElement element : elements) {
            if (element.getCommand().isEmpty()) {
                CommandGrapher.CommandGraphElement tmp = element;
                while (tmp.getCommand().isEmpty() && tmp.getCommand().getEndLink().getOnSuccess() != null) {
                    tmp = commands.get(tmp.getCommand().getEndLink().getOnSuccess());
                }
                if ((!tmp.getCommand().isEmpty() || tmp.getCommand().getEndLink().getOnSuccess() != null) && element.getDepth().getDepth() != commands.get(
                                element.getCommand().getEndLink().getOnSuccess())
                        .getDepth().getDepth()) {
                    compactedElements.add(element);
                }
            } else {
                compactedElements.add(element);
            }
        }
        return compactedElements;
    }

}