package net.zscript.javaclient.core;

import java.io.ByteArrayOutputStream;
import java.io.OutputStream;
import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.List;
import java.util.Optional;

import net.zscript.model.ZscriptModel;
import net.zscript.model.datamodel.ZscriptDataModel;

public class Command {
    public static class CommandEndLink {
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

    private final CommandEndLink  endLink;
    private final ZscriptFieldSet fieldSet;

    Command(CommandEndLink endLink, ZscriptFieldSet fieldSet) {
        this.endLink = endLink;
        this.fieldSet = fieldSet;
    }

    public CommandEndLink getEndLink() {
        return endLink;
    }

    public void toBytes(OutputStream out) {
        fieldSet.toBytes(out);
    }

    public boolean isEmpty() {
        return fieldSet.isEmpty();
    }

    public boolean canFail() {
        if (isEmpty()) {
            return false;
        }
        if (fieldSet.getFieldValue('Z') == 1) {
            int sVal = fieldSet.getFieldValue('S');
            if (sVal > 0x00 && sVal < 0x10) {
                return true;
            } else {
                return false;
            }
        }
        return true;
    }

    public boolean canSucceed() {
        if (fieldSet.getFieldValue('Z') == 1) {
            int sVal = fieldSet.getFieldValue('S');
            if (sVal == 0 || sVal == -1) {
                return true;
            } else {
                return false;
            }
        }
        return true;
    }

    @Override
    public String toString() {
        ByteArrayOutputStream outputStream = new ByteArrayOutputStream();
        toBytes(outputStream);
        return StandardCharsets.UTF_8.decode(ByteBuffer.wrap(outputStream.toByteArray())).toString();
    }

    public static class PrintSettings {
        private final int              lineWidth;
        private final String           indentString;
        private final VerbositySetting verbosity;

        public PrintSettings(int lineWidth, String indentString, VerbositySetting verbosity) {
            this.lineWidth = lineWidth;
            this.indentString = indentString;
            this.verbosity = verbosity;
            if (lineWidth - indentString.length() * 5 < 20) {
                throw new IllegalArgumentException("Line width too short for given indent");
            }
        }

        public VerbositySetting getVerbosity() {
            return verbosity;
        }

        public String lineWrap(List<String> lines, List<Integer> indents) {
            StringBuilder builder = new StringBuilder();
            for (int i = 0; i < lines.size(); i++) {
                String line     = lines.get(i);
                String indent   = indentString.repeat(indents.get(i));
                int    progress = lineWidth - indent.length();
                for (int currentPos = 0; currentPos < line.length(); ) {
                    while (Character.isWhitespace(line.charAt(currentPos))) {
                        currentPos++;
                    }
                    boolean needsHyphen = false;
                    int     nextPos     = Math.min(currentPos + progress, line.length());
                    while (!(nextPos == line.length() || Character.isWhitespace(line.charAt(nextPos)))) {
                        nextPos--;
                    }
                    if (nextPos == currentPos) {
                        nextPos = currentPos + progress - 1;
                        needsHyphen = true;
                    }
                    builder.append(indent);
                    builder.append(line, currentPos, nextPos);
                    if (needsHyphen) {
                        builder.append('-');
                    }
                    builder.append('\n');
                    currentPos = nextPos;
                }
            }
            return builder.toString();
        }
    }

    private String toSentence(String desc) {
        return desc.substring(0, 1).toUpperCase() + desc.substring(1) + '.';
    }

    private String upperFirst(String name) {
        return name.substring(0, 1).toUpperCase() + name.substring(1);
    }

    public String explain(ZscriptModel model, PrintSettings settings) {
        List<String>  lines   = new ArrayList<>();
        List<Integer> indents = new ArrayList<>();

        StringBuilder builder = new StringBuilder();
        indents.add(0);
        builder.append(toString());
        if (settings.getVerbosity().compareTo(VerbositySetting.MINIMAL) <= 0) {
            lines.add(builder.toString());
            return settings.lineWrap(lines, indents);
        }
        builder.append(": ");
        int commandValue = fieldSet.getFieldValue('Z');
        if (commandValue == -1) {
            builder.append("No command field - Z expected");
            lines.add(builder.toString());
            return settings.lineWrap(lines, indents);
        }
        Optional<ZscriptDataModel.CommandModel> commandOpt = model.getCommand(commandValue);
        if (commandOpt.isEmpty()) {
            builder.append("Command not recognised");
            lines.add(builder.toString());
            return settings.lineWrap(lines, indents);
        }
        ZscriptDataModel.CommandModel command = commandOpt.get();
        builder.append(upperFirst(command.getCommandName()));
        if (settings.getVerbosity().compareTo(VerbositySetting.NAME) <= 0) {
            lines.add(builder.toString());
            return settings.lineWrap(lines, indents);
        }
        lines.add(builder.toString());

        if (settings.getVerbosity().compareTo(VerbositySetting.DESCRIPTIONS) >= 0) {
            indents.add(2);
            lines.add(toSentence(command.getDescription()));
        }
        boolean[] doneFields = new boolean[26];
        for (ZscriptDataModel.RequestFieldModel field : command.getRequestFields()) {
            if (field.getKey() == 'Z') {
                doneFields[field.getKey() - 'A'] = true;
                continue;
            }
            indents.add(1);
            builder = new StringBuilder();
            char key = field.getKey();
            builder.append(upperFirst(field.getName()));
            builder.append(": ");
            if (field.getTypeDefinition() instanceof ZscriptDataModel.BigfieldTypeDefinition) {
                byte[] bigFields = fieldSet.getBigFieldTotal();
                if (bigFields.length == 0) {
                    builder.append("No data");
                } else {
                    if (field.getTypeDefinition() instanceof ZscriptDataModel.TextTypeDefinition) {
                        builder.append('"');
                        builder.append(StandardCharsets.UTF_8.decode(ByteBuffer.wrap(bigFields)));
                        builder.append('"');
                    } else {
                        builder.append("(Hex) ");
                        for (byte b : bigFields) {
                            builder.append((char) ZscriptFieldSet.hexchar((byte) (b >> 4)));
                            builder.append((char) ZscriptFieldSet.hexchar((byte) (b & 0xF)));
                            builder.append(" ");
                        }
                    }
                }

                lines.add(builder.toString());
                if (settings.getVerbosity().compareTo(VerbositySetting.DESCRIPTIONS) >= 0) {
                    indents.add(3);
                    lines.add(toSentence(field.getDescription()));
                }
            } else {
                doneFields[key - 'A'] = true;
                int value = fieldSet.getFieldValue(key);
                if (value == -1) {
                    if (field.isRequired()) {
                        builder.append("Is required but missing");
                    } else {
                        builder.append("Not present");
                    }
                } else {
                    if (field.getTypeDefinition() instanceof ZscriptDataModel.FlagTypeDefinition) {
                        builder.append("Present");
                    } else {
                        builder.append("0x");
                        ZscriptFieldSet.hexString(builder, value, 1);
                    }
                    if (field.getTypeDefinition() instanceof ZscriptDataModel.AnyTypeDefinition
                            || field.getTypeDefinition() instanceof ZscriptDataModel.NumberTypeDefinition) {
                        builder.append(" (");
                        builder.append(value);
                        builder.append(")");
                    } else if (field.getTypeDefinition() instanceof ZscriptDataModel.EnumTypeDefinition) {
                        builder.append(" (");
                        List<String> values = ((ZscriptDataModel.EnumTypeDefinition) field.getTypeDefinition()).getValues();
                        if (value >= values.size()) {
                            builder.append("Out of range");
                        } else {
                            builder.append(upperFirst(values.get(value)));
                        }
                        builder.append(")");
                    }
                }
                lines.add(builder.toString());
                if (settings.getVerbosity().compareTo(VerbositySetting.DESCRIPTIONS) >= 0) {
                    indents.add(3);
                    lines.add(toSentence(field.getDescription()));
                }
                if (settings.getVerbosity().compareTo(VerbositySetting.BITFIELDS) >= 0) {
                    if (field.getTypeDefinition() instanceof ZscriptDataModel.BitsetTypeDefinition) {
                        List<ZscriptDataModel.BitsetTypeDefinition.Bit> bits = ((ZscriptDataModel.BitsetTypeDefinition) field.getTypeDefinition()).getBits();
                        int                                             i    = 0;
                        for (ZscriptDataModel.BitsetTypeDefinition.Bit bit : bits) {
                            indents.add(2);
                            builder = new StringBuilder();
                            builder.append(upperFirst(bit.getName()));
                            if (value != -1) {
                                builder.append(": ");
                                if ((value & (1 << i)) != 0) {
                                    builder.append("True");
                                } else {
                                    builder.append("False");
                                }
                            }
                            lines.add(builder.toString());
                            if (settings.getVerbosity().compareTo(VerbositySetting.DESCRIPTIONS) >= 0) {
                                indents.add(4);
                                lines.add(toSentence(bit.getDescription()));
                            }
                            i++;
                        }
                    }
                }
            }
        }
        for (int i = 0; i < 26; i++) {
            if (!doneFields[i] && fieldSet.getFieldValue((byte) ('A' + i)) != -1) {
                indents.add(1);
                builder = new StringBuilder();
                builder.append("Unknown Field ");
                builder.append((char) ('A' + i));
                builder.append(" with value: ");
                builder.append("0x");
                ZscriptFieldSet.hexString(builder, fieldSet.getFieldValue((byte) ('A' + i)), 1);
                lines.add(builder.toString());
            }
        }
        return settings.lineWrap(lines, indents);
    }

}
