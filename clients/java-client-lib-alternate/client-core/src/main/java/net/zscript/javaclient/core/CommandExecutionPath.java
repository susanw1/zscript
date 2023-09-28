package net.zscript.javaclient.core;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.Collections;
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
import net.zscript.model.datamodel.ZscriptDataModel;

public class CommandExecutionPath {
    public static int hexchar(byte nibble) {
        if (nibble > 9) {
            return 'a' + nibble - 10;
        } else {
            return '0' + nibble;
        }
    }

    public static void hexString(OutputStream out, int value) {
        try {
            if (value > 0xFFFF) {
                throw new IllegalStateException();
            }
            if (value > 0xFFF) {
                out.write(hexchar((byte) ((value >> 12) & 0xF)));
            }
            if (value > 0xFF) {
                out.write(hexchar((byte) ((value >> 8) & 0xF)));
            }
            if (value > 0xF) {
                out.write(hexchar((byte) ((value >> 4) & 0xF)));
            }
            if (value > 0) {
                out.write(hexchar((byte) (value & 0xF)));
            }
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

    static class Command {
        private final CommandEndLink endLink;

        private final Map<Byte, Integer> fields;
        private final List<byte[]>       bigFields;

        private final boolean hasClash;

        Command(CommandEndLink endLink, Map<Byte, Integer> fields, List<byte[]> bigFields, boolean hasClash) {
            this.endLink = endLink;
            this.fields = fields;
            this.bigFields = bigFields;
            this.hasClash = hasClash;
        }

        public void toBytes(OutputStream out) {
            try {
                if (fields.containsKey((Byte) (byte) 'Z')) {
                    out.write('Z');
                    hexString(out, fields.get((Byte) (byte) 'Z'));
                }
                for (Map.Entry<Byte, Integer> entry : fields.entrySet()) {
                    if (entry.getKey() != (Byte) (byte) 'Z') {
                        out.write(entry.getKey());
                        hexString(out, entry.getValue());
                    }
                }
                for (byte[] bytes : bigFields) {
                    out.write('+');
                    for (byte b : bytes) {
                        out.write(hexchar((byte) (b >> 4)));
                        out.write(hexchar((byte) (b & 0xF)));
                    }
                }
            } catch (IOException e) {
                throw new RuntimeException(e);
            }
        }

        public CommandEndLink getEndLink() {
            return endLink;
        }
    }

    static class CommandEndLink {
        private final Command onSuccess;
        private final byte[]  successSeparators;
        private final Command onFail;
        private final byte[]  failSeparators;

        CommandEndLink(Command onSuccess, byte[] successSeparators, Command onFail, byte[] failSeparators) {
            this.onSuccess = onSuccess;
            this.successSeparators = successSeparators;
            this.onFail = onFail;
            this.failSeparators = failSeparators;
        }

        public Command getOnSuccess() {
            return onSuccess;
        }

        public byte[] getSuccessSeparators() {
            return successSeparators;
        }

        public Command getOnFail() {
            return onFail;
        }

        public byte[] getFailSeparators() {
            return failSeparators;
        }

    }

    static class CommandBuilder {
        TokenBuffer.TokenReader.ReadToken start = null;

        CommandBuilder onSuccess           = null;
        int            successBracketCount = 0;
        List<Byte>     successSeperators   = new ArrayList<>();

        CommandBuilder onFail           = null;
        int            failBracketCount = 0;
        List<Byte>     failSeperators   = new ArrayList<>();

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

        private CommandEndLink generateLinks(Map<CommandBuilder, Command> commands) {
            byte[] successSepArr = new byte[successSeperators.size()];

            int i = 0;
            for (byte b : successSeperators) {
                successSepArr[i++] = b;
            }
            byte[] failSepArr = new byte[failSeperators.size()];
            i = 0;
            for (byte b : failSeperators) {
                failSepArr[i++] = b;
            }
            return new CommandEndLink(commands.get(onSuccess), successSepArr, commands.get(onFail), failSepArr);
        }

        public Command generateCommand(Map<CommandBuilder, Command> otherCommands) {
            CommandEndLink     link      = generateLinks(otherCommands);
            Map<Byte, Integer> fields    = new HashMap<>();
            List<byte[]>       bigFields = new ArrayList<>();

            boolean hasClash = false;

            TokenBufferIterator iterator = start.getNextTokens();
            for (Optional<TokenBuffer.TokenReader.ReadToken> opt = iterator.next(); opt.filter(t -> !t.isMarker()).isPresent(); opt = iterator.next()) {
                TokenBuffer.TokenReader.ReadToken token = opt.get();
                if (Zchars.isBigField(token.getKey())) {
                    byte[] data = new byte[token.getDataSize()];

                    int i = 0;
                    for (Iterator<Byte> iter = token.blockIterator(); iter.hasNext(); ) {
                        data[i++] = iter.next();
                    }
                    bigFields.add(data);
                } else {
                    if (fields.containsKey(token.getKey()) || token.getDataSize() > 2) {
                        hasClash = true;
                    } else {
                        fields.put(token.getKey(), token.getData16());
                    }
                }
            }
            return new Command(link, Collections.unmodifiableMap(fields), Collections.unmodifiableList(bigFields), hasClash);
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
                    last.successSeperators.add(Zchars.Z_ANDTHEN);
                    needFailPath.add(last);
                } else if (token.getKey() == Tokenizer.CMD_END_ORELSE) {
                    needSuccessPath.add(last);
                    last.setOnFail(next);
                    last.failSeperators.add(Zchars.Z_ORELSE);
                    for (Iterator<CommandBuilder> iter = needFailPath.iterator(); iter.hasNext(); ) {
                        CommandBuilder b = iter.next();
                        if (b.failBracketCount == 0) {
                            b.setOnFail(next);
                            b.failSeperators.add(Zchars.Z_ORELSE);
                            iter.remove();
                        }
                    }
                } else if (token.getKey() == Tokenizer.CMD_END_OPEN_PAREN) {
                    last.setOnSuccess(next);
                    needFailPath.add(last);
                    last.successSeperators.add(Zchars.Z_OPENPAREN);
                    for (CommandBuilder b : needFailPath) {
                        b.failBracketCount++;
                    }
                    for (CommandBuilder b : needSuccessPath) {
                        b.successBracketCount++;
                    }
                } else if (token.getKey() == Tokenizer.CMD_END_CLOSE_PAREN) {
                    last.setOnSuccess(next);
                    needFailPath.add(last);
                    last.successSeperators.add(Zchars.Z_CLOSEPAREN);
                    for (CommandBuilder b : needFailPath) {
                        if (b.failBracketCount > 0) {
                            b.failBracketCount--;
                            b.failSeperators.add(Zchars.Z_CLOSEPAREN);
                        }
                    }
                    for (Iterator<CommandBuilder> iter = needSuccessPath.iterator(); iter.hasNext(); ) {
                        CommandBuilder b = iter.next();
                        if (b.successBracketCount == 0) {
                            b.setOnSuccess(next);
                            b.successSeperators.add(Zchars.Z_CLOSEPAREN);
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
        return parse(null, start);
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

    public byte[] toSequence() {
        ByteArrayOutputStream out = new ByteArrayOutputStream();

        Command current = firstCommand;
        while (true) {
            current.toBytes(out);
            CommandEndLink link = current.getEndLink();
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

}