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

import static net.zscript.tokenizer.TokenBuffer.TokenReader.ReadToken;

import net.zscript.model.ZscriptModel;
import net.zscript.model.components.Zchars;
import net.zscript.tokenizer.TokenBufferIterator;
import net.zscript.tokenizer.Tokenizer;
import net.zscript.util.ByteString.ByteAppendable;
import net.zscript.util.ByteString.ByteStringBuilder;
import net.zscript.util.OptIterator;

public class CommandExecutionPath implements Iterable<Command>, ByteAppendable {

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

    public static CommandExecutionPath from(ZscriptModel model, Command success) {
        return new CommandExecutionPath(model, success);
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

    private static List<CommandBuilder> createLinkedPaths(ReadToken start) {
        List<CommandBuilder> builders = new ArrayList<>();

        List<CommandBuilder> needSuccessPath = new ArrayList<>();
        List<CommandBuilder> needFailPath    = new ArrayList<>();

        CommandBuilder last = new CommandBuilder();
        builders.add(last);
        if (start == null) {
            return builders;
        }

        TokenBufferIterator iterator = start.tokenIterator();
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

    @Override
    public void appendTo(ByteStringBuilder builder) {
        Deque<Command> openedTrees  = new ArrayDeque<>();
        Deque<Command> workingTrees = new ArrayDeque<>();
        if (firstCommand != null) {
            workingTrees.push(firstCommand);
        }
        boolean needsAnd  = false;
        boolean needsOpen = false;
        while (!workingTrees.isEmpty()) {
            Command current = workingTrees.peek();

            Command latestOpenTree = openedTrees.peek();
            if (latestOpenTree != null && current.getOnFail() != latestOpenTree) {
                // if there are opened trees, and this command doesn't add to the top open tree,
                //   check if it closes the top open tree
                //   which we can do by iterating forward to see if the top open tree re-merges here
                // this operation makes the method O(n^2)
                Command tmp        = latestOpenTree;
                boolean mergesHere = false;
                while (tmp != null) {
                    if (tmp == current) {
                        mergesHere = true;
                        break;
                    }
                    tmp = tmp.getOnSuccess();
                }
                if (mergesHere) {
                    openedTrees.pop();
                    workingTrees.push(latestOpenTree);
                    builder.appendByte(Zchars.Z_ORELSE);
                    needsOpen = false;
                    needsAnd = false;
                    // if it closes the top open tree, process that tree first
                    continue;
                }
            }
            if (current.getOnFail() != null && current.getOnFail() != latestOpenTree) {
                // the command opens a new open tree
                if (needsOpen) {
                    builder.appendByte(Zchars.Z_OPENPAREN);
                }
                openedTrees.push(current.getOnFail());
                latestOpenTree = current.getOnFail();
            } else if (needsAnd) {
                builder.appendByte(Zchars.Z_ANDTHEN);
            }

            workingTrees.pop().appendTo(builder); // only put elements into the list when we're done processing them
            needsAnd = false;
            needsOpen = true;
            if (current.getOnSuccess() != null) {
                if (workingTrees.isEmpty() || current.getOnSuccess() != workingTrees.peek()) {
                    // check we're not on a close parent, dropping out of failure
                    workingTrees.push(current.getOnSuccess());
                    needsAnd = true;
                } else {
                    builder.appendByte(Zchars.Z_CLOSEPAREN);
                }
            } else if (latestOpenTree != null) {
                openedTrees.pop();
                workingTrees.push(latestOpenTree);
                builder.appendByte(Zchars.Z_ORELSE);
                needsOpen = false;
            }
        }
    }

    public boolean matchesResponses(ResponseExecutionPath resps) {
        try {
            compareResponses(resps);
            return true;
        } catch (IllegalArgumentException ex) {
            return false;
        }
    }

    public List<MatchedCommandResponse> compareResponses(ResponseExecutionPath resps) {
        Deque<Command> parenStarts = new ArrayDeque<>();
        // fill out parenStarts so that we can have as many ')' as we want...
        Command tmp1 = firstCommand;
        while (tmp1 != null) {
            parenStarts.add(tmp1);
            tmp1 = tmp1.getOnFail();
        }

        boolean lastEndedOpen  = false;
        boolean lastEndedClose = false;
        boolean lastSucceeded  = true;
        int     lastParenCount = 0;

        List<MatchedCommandResponse> cmds        = new ArrayList<>();
        Command                      currentCmd  = firstCommand;
        Response                     currentResp = resps.getFirstResponse();
        Command                      lastFail    = null;

        while (currentResp != null) {
            if (currentCmd == null) {
                throw new IllegalArgumentException("Command sequence ended before response - cannot match");
            }
            cmds.add(new MatchedCommandResponse(currentCmd, currentResp));

            if (lastSucceeded) {
                if (lastEndedClose) {
                    if (parenStarts.peek().getOnFail() == currentCmd.getOnFail()) {
                        throw new IllegalArgumentException("Response has ')' without valid opening '('");
                    }
                    Command tmp2 = parenStarts.pop().getOnFail();
                    while (tmp2 != null && tmp2 != currentCmd) {
                        tmp2 = tmp2.getOnSuccess();
                    }
                    if (tmp2 != currentCmd) {
                        throw new IllegalArgumentException("Response has ')' without command sequence merging");
                    }
                    lastEndedClose = false;
                } else if (lastEndedOpen) {
                    parenStarts.push(currentCmd);
                    lastEndedOpen = false;
                } else if (lastFail != null && currentCmd.getOnFail() != lastFail) {
                    throw new IllegalArgumentException("Fail conditions don't match up around '&'");
                }
            } else {
                for (int i = 0; i < lastParenCount; i++) {
                    if (parenStarts.isEmpty()) {
                        throw new IllegalArgumentException("Command sequence ran out of parens before response sequence");
                    }
                    Command tmp3 = parenStarts.peek().getOnFail();
                    while (tmp3 != null && tmp3.getOnFail() != currentCmd) {
                        tmp3 = tmp3.getOnSuccess();
                    }
                    if (tmp3 == null) {
                        throw new IllegalArgumentException("Response has ')' without command sequence merging");
                    }
                    tmp3 = parenStarts.peek().getOnFail();
                    while (tmp3 != null && tmp3 != currentCmd) {
                        tmp3 = tmp3.getOnFail();
                    }
                    if (tmp3 != currentCmd) {
                        throw new IllegalArgumentException("Response has ')' without command sequence merging");
                    }
                    parenStarts.pop();
                }
                if (parenStarts.isEmpty() || parenStarts.peek().getOnFail() != currentCmd) {
                    throw new IllegalArgumentException("Response has failure divergence without parenthesis");
                }
            }
            if (currentResp.wasSuccess()) {
                if (currentResp.hasCloseParen()) {
                    lastEndedClose = true;
                } else if (currentResp.hasOpenParen()) {
                    lastEndedOpen = true;
                }
                lastFail = currentCmd.getOnFail();
                lastSucceeded = true;
                currentCmd = currentCmd.getOnSuccess();
            } else {
                lastFail = null;
                lastParenCount = currentResp.getParenCount();
                lastSucceeded = false;
                currentCmd = currentCmd.getOnFail();
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
                        if (c != null && !visited.contains(c)) {
                            visited.add(c);
                            Command s = c.getOnSuccess();
                            if (!visited.contains(s)) {
                                next.add(s);
                            }
                            Command f = c.getOnFail();
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

    private static class CommandBuilder {
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

        public Command generateCommand(Map<CommandBuilder, Command> commands) {
            return new Command(commands.get(onSuccess), commands.get(onFail), ZscriptFieldSet.fromTokens(start));
        }
    }
}
