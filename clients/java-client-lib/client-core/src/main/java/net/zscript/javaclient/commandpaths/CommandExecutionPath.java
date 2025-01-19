package net.zscript.javaclient.commandpaths;

import javax.annotation.Nonnull;
import javax.annotation.Nullable;
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
import java.util.function.Consumer;

import static java.lang.Byte.toUnsignedInt;
import static java.lang.Integer.toHexString;
import static net.zscript.tokenizer.TokenBuffer.TokenReader.ReadToken;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import net.zscript.javaclient.ZscriptMismatchedResponseException;
import net.zscript.javaclient.ZscriptParseException;
import net.zscript.model.ZscriptModel;
import net.zscript.model.components.Zchars;
import net.zscript.tokenizer.TokenBufferIterator;
import net.zscript.tokenizer.Tokenizer;
import net.zscript.util.ByteString.ByteAppendable;
import net.zscript.util.ByteString.ByteStringBuilder;
import net.zscript.util.OptIterator;

/**
 * A sendable, unaddressed command sequence, without locks and echo fields. It is composed of {@link CommandElement} objects which are aware of what Commands follow on
 * success/failure, so a CommandExecutionPath is essentially "execution-aware".
 * <p>
 * It's the bare "thing that we want executing", before adding the addressing and sequence-level fields which get it to the right place and in the right state. These might be added
 * by a client runtime representing a device node hierarchy, see {@link net.zscript.javaclient.devicenodes.ZscriptNode#send(CommandExecutionPath, Consumer)}.
 * <p>
 * On execution, the corresponding response object is a {@link ResponseExecutionPath}.
 */
public class CommandExecutionPath implements Iterable<CommandElement>, ByteAppendable {
    private static final Logger LOG = LoggerFactory.getLogger(CommandExecutionPath.class);

    public static CommandExecutionPath parse(ReadToken start) {
        return parse(ZscriptModel.standardModel(), start);
    }

    public static CommandExecutionPath parse(ZscriptModel model, @Nullable ReadToken start) {
        final List<CommandBuilder> builders = createLinkedPaths(start);

        final Map<CommandBuilder, CommandElement> commands = new HashMap<>();
        for (ListIterator<CommandBuilder> iter = builders.listIterator(builders.size()); iter.hasPrevious(); ) {
            final CommandBuilder b = iter.previous();
            if (b.start != null) {
                commands.put(b, b.generateCommand(commands));
            }
        }
        return new CommandExecutionPath(model, commands.get(builders.get(0)));
    }

    public static CommandExecutionPath from(ZscriptModel model, CommandElement success) {
        return new CommandExecutionPath(model, success);
    }

    public static CommandExecutionPath blank() {
        return blank(ZscriptModel.standardModel());
    }

    public static CommandExecutionPath blank(ZscriptModel model) {
        return new CommandExecutionPath(model, null);
    }

    private final ZscriptModel   model;
    private final CommandElement firstCommand;

    private CommandExecutionPath(ZscriptModel model, @Nullable CommandElement firstCommand) {
        this.model = model;
        this.firstCommand = firstCommand;
    }

    /**
     * Utility method used in command-path generation. Extracts all the commands starting from the supplied token, building a binary tree linking each command to its
     * successor-on-success and successor-on-failure.
     *
     * @param start the token representing the first command
     * @return a list of linked command-builders, ready for full path generation
     */
    private static List<CommandBuilder> createLinkedPaths(@Nullable ReadToken start) {
        final List<CommandBuilder> builders = new ArrayList<>();

        // working list of commands which still need a success/failure successor
        final List<CommandBuilder> needSuccessPath = new ArrayList<>();
        final List<CommandBuilder> needFailPath    = new ArrayList<>();

        CommandBuilder last = new CommandBuilder();
        builders.add(last);
        if (start == null) {
            return builders;
        }

        final TokenBufferIterator iterator = start.tokenIterator();
        for (Optional<ReadToken> opt = iterator.next(); opt.isPresent(); opt = iterator.next()) {
            final ReadToken token = opt.get();
            if (last.getStart() == null) {
                last.setStart(token);
            }
            if (token.isMarker()) {
                final CommandBuilder next = new CommandBuilder();
                builders.add(next);
                if (token.getKey() == Tokenizer.CMD_END_ANDTHEN) {
                    last.setOnSuccess(next);
                    needFailPath.add(last);
                } else if (token.getKey() == Tokenizer.CMD_END_ORELSE) {
                    needSuccessPath.add(last);
                    last.setOnFail(next);
                    for (Iterator<CommandBuilder> iter = needFailPath.iterator(); iter.hasNext(); ) {
                        final CommandBuilder b = iter.next();
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
                        final CommandBuilder b = iter.next();
                        if (b.successBracketCount == 0) {
                            b.setOnSuccess(next);
                            iter.remove();
                        } else {
                            b.successBracketCount--;
                        }
                    }
                } else if (token.isSequenceEndMarker()) {
                    if (token.getKey() != Tokenizer.NORMAL_SEQUENCE_END) {
                        throw new ZscriptParseException("Syntax error [marker=%s, token=%s]", token, start);
                    }
                    break;
                } else {
                    throw new ZscriptParseException("Unknown separator [key=%s, token=%s]", toHexString(toUnsignedInt(token.getKey())), token);
                }
                last = next;
            }
        }
        return builders;
    }

    @Override
    public void appendTo(ByteStringBuilder builder) {
        final Deque<CommandElement> openedTrees  = new ArrayDeque<>();
        final Deque<CommandElement> workingTrees = new ArrayDeque<>();
        if (firstCommand != null) {
            workingTrees.push(firstCommand);
        }
        boolean needsAnd  = false;
        boolean needsOpen = false;
        while (!workingTrees.isEmpty()) {
            final CommandElement current = workingTrees.peek();

            CommandElement latestOpenTree = openedTrees.peek();
            if (latestOpenTree != null && current.getOnFail() != latestOpenTree) {
                // if there are opened trees, and this command doesn't add to the top open tree,
                //   check if it closes the top open tree
                //   which we can do by iterating forward to see if the top open tree re-merges here
                // this operation makes the method O(n^2)
                CommandElement tmp        = latestOpenTree;
                boolean        mergesHere = false;
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

    /**
     * Determines whether the supplied ResponseExecutionPath is a plausible response to these commands.
     *
     * @param resps a response path to check
     * @return true if they match, false otherwise
     */
    public boolean matchesResponses(ResponseExecutionPath resps) {
        try {
            compareResponses(resps);
            return true;
        } catch (ZscriptMismatchedResponseException ex) {
            LOG.atTrace().addArgument(() -> ex).log("Mismatch identified: {}");
            return false;
        }
    }

    /**
     * Creates a list of response matches from the supplied response path, corresponding to this command path.
     *
     * @param resps a response path to match
     * @return a list of these commands, matched to their corresponding responses
     * @throws ZscriptMismatchedResponseException if the supplied response doesn't match this command sequence
     */
    public List<MatchedCommandResponse> compareResponses(ResponseExecutionPath resps) {
        final Deque<CommandElement> parenStarts = new ArrayDeque<>();
        // fill out parenStarts so that we can have as many ')' as we want...
        CommandElement tmp1 = firstCommand;
        while (tmp1 != null) {
            parenStarts.add(tmp1);
            tmp1 = tmp1.getOnFail();
        }

        boolean lastEndedOpen  = false;
        boolean lastEndedClose = false;
        boolean lastSucceeded  = true;
        int     lastParenCount = 0;

        final List<MatchedCommandResponse> cmds        = new ArrayList<>();
        CommandElement                     currentCmd  = firstCommand;
        ResponseElement                    currentResp = resps.getFirstResponse();
        CommandElement                     lastFail    = null;

        while (currentResp != null && currentCmd != null) {
            cmds.add(new MatchedCommandResponse(currentCmd, currentResp));

            if (lastSucceeded) {
                if (lastEndedClose) {
                    if (parenStarts.peek().getOnFail() == currentCmd.getOnFail()) {
                        throw new ZscriptMismatchedResponseException("Response has ')' without valid opening '('");
                    }
                    CommandElement tmp2 = parenStarts.pop().getOnFail();
                    while (tmp2 != null && tmp2 != currentCmd) {
                        tmp2 = tmp2.getOnSuccess();
                    }
                    if (tmp2 != currentCmd) {
                        throw new ZscriptMismatchedResponseException("Response has ')' without command sequence merging");
                    }
                    lastEndedClose = false;
                } else if (lastEndedOpen) {
                    parenStarts.push(currentCmd);
                    lastEndedOpen = false;
                } else if (lastFail != null && currentCmd.getOnFail() != lastFail) {
                    throw new ZscriptMismatchedResponseException("Fail conditions don't match up around '&'");
                }
            } else {
                for (int i = 0; i < lastParenCount; i++) {
                    if (parenStarts.isEmpty()) {
                        throw new ZscriptMismatchedResponseException("Command sequence ran out of parens before response sequence");
                    }
                    CommandElement tmp3 = parenStarts.peek().getOnFail();
                    while (tmp3 != null && tmp3.getOnFail() != currentCmd) {
                        tmp3 = tmp3.getOnSuccess();
                    }
                    if (tmp3 == null) {
                        throw new ZscriptMismatchedResponseException("Response has ')' without command sequence merging");
                    }
                    tmp3 = parenStarts.peek().getOnFail();
                    while (tmp3 != null && tmp3 != currentCmd) {
                        tmp3 = tmp3.getOnFail();
                    }
                    if (tmp3 != currentCmd) {
                        throw new ZscriptMismatchedResponseException("Response has ')' without command sequence merging");
                    }
                    parenStarts.pop();
                }
                // We used to check this, but we don't have a test-case and it rejects AS|BS2|C because parenStarts doesn't progress. Disable for now.
                //                if (parenStarts.isEmpty() || parenStarts.peek().getOnFail() != currentCmd) {
                //                    throw new ZscriptMismatchedResponseException("Response has failure divergence without parenthesis");
                //                }
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

        if (currentCmd == null && currentResp != null && (!currentResp.getFields().isEmpty() || currentResp.getNext() != null)) {
            throw new ZscriptMismatchedResponseException("Command sequence ended before response - cannot match");
        }

        return cmds;
    }

    @Nonnull
    public Iterator<CommandElement> iterator() {
        if (firstCommand == null) {
            return Collections.emptyListIterator();
        }
        return new OptIterator<CommandElement>() {
            final Set<CommandElement> visited = new HashSet<>();
            Iterator<CommandElement> toVisit = Set.of(firstCommand).iterator();
            Set<CommandElement>      next    = new HashSet<>();

            @Nonnull
            @Override
            public Optional<CommandElement> next() {
                while (true) {
                    while (toVisit.hasNext()) {
                        CommandElement c = toVisit.next();
                        if (c != null && !visited.contains(c)) {
                            visited.add(c);
                            CommandElement s = c.getOnSuccess();
                            if (!visited.contains(s)) {
                                next.add(s);
                            }
                            CommandElement f = c.getOnFail();
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
        for (CommandElement c : this) {
            length += c.getBufferLength() + 1;
        }
        return length;
    }

    public CommandElement getFirstCommand() {
        return firstCommand;
    }

    public ZscriptModel getModel() {
        return model;
    }

    @Override
    public String toString() {
        return toStringImpl();
    }

    /**
     * Utility class representing a command element during the parse process, finally producing a Command object which knows which commands it will go to on success and on
     * failure.
     */
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

        public CommandElement generateCommand(Map<CommandBuilder, CommandElement> commands) {
            return new CommandElement(commands.get(onSuccess), commands.get(onFail), ZscriptFieldSet.fromTokens(start));
        }
    }
}
