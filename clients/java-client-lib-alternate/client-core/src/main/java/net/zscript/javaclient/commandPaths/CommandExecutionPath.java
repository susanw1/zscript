package net.zscript.javaclient.commandPaths;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.ListIterator;
import java.util.Map;
import java.util.Optional;

import static net.zscript.javareceiver.tokenizer.TokenBuffer.TokenReader.ReadToken;

import net.zscript.javaclient.commandbuilder.ZscriptByteString;
import net.zscript.javareceiver.tokenizer.TokenBuffer;
import net.zscript.javareceiver.tokenizer.TokenBufferIterator;
import net.zscript.javareceiver.tokenizer.Tokenizer;
import net.zscript.model.ZscriptModel;
import net.zscript.model.components.Zchars;
import net.zscript.util.ByteString;

public class CommandExecutionPath {

    static class CommandBuilder {
        ReadToken start = null;

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

        public ReadToken getStart() {
            return start;
        }

        public void setStart(ReadToken token) {
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

    public ByteString toSequence() {
        ZscriptByteString.ZscriptByteStringBuilder out = ZscriptByteString.builder();

        Command current = firstCommand;
        while (true) {
            current.toBytes(out);
            Command.CommandEndLink link = current.getEndLink();
            if (link.getOnSuccess() == null) {
                if (link.getOnFail() == null) {
                    break;
                } else {
                    out.appendByte(link.getFailSeparators()[0]);
                    current = link.getOnFail();
                }
            } else if (link.getSuccessSeparators()[0] == Zchars.Z_CLOSEPAREN) {
                if (link.getOnFail() == null || link.getFailSeparators()[0] == Zchars.Z_CLOSEPAREN) {
                    out.appendByte(Zchars.Z_CLOSEPAREN);
                    current = link.getOnSuccess();
                } else {
                    out.appendByte(Zchars.Z_ORELSE);
                    current = link.getOnFail();
                }
            } else {
                out.appendByte(link.getSuccessSeparators()[0]);
                current = link.getOnSuccess();
            }
        }
        return out.build();
    }

    public Command getFirstCommand() {
        return firstCommand;
    }

    public ZscriptModel getModel() {
        return model;
    }
}