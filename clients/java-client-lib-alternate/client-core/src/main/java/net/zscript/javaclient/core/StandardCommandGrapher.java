package net.zscript.javaclient.core;

import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.Optional;

import net.zscript.model.ZscriptModel;
import net.zscript.model.datamodel.ZscriptDataModel;

public class StandardCommandGrapher implements CommandGrapher {
    public static final String ANSI_RESET  = "\u001B[0m";
    public static final String ANSI_BOLD   = "\u001B[1m";
    public static final String ANSI_BLACK  = "\u001B[30m";
    public static final String ANSI_RED    = "\u001B[31m";
    public static final String ANSI_GREEN  = "\u001B[32m";
    public static final String ANSI_YELLOW = "\u001B[33m";
    public static final String ANSI_BLUE   = "\u001B[34m";
    public static final String ANSI_PURPLE = "\u001B[35m";
    public static final String ANSI_CYAN   = "\u001B[36m";
    public static final String ANSI_WHITE  = "\u001B[37m";

    public static final String actualPath = ANSI_BOLD + ANSI_CYAN;

    public static class CommandPrintSettings {
        private final int              lineWidth;
        private final String           indentString;
        private final VerbositySetting verbosity;

        public CommandPrintSettings(int lineWidth, String indentString, VerbositySetting verbosity) {
            this.lineWidth = lineWidth;
            this.indentString = indentString;
            this.verbosity = verbosity;
            if (lineWidth - indentString.length() * 4 < 20) {
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
                    while (!(nextPos == 0 || nextPos == line.length() || Character.isWhitespace(line.charAt(nextPos)))) {
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

    private void explainBigField(List<String> lines, List<Integer> indents, ZscriptDataModel.GenericField field, ZscriptFieldSet fieldSet,
            CommandPrintSettings settings, StringBuilder builder) {
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
    }

    private void explainBitField(List<String> lines, List<Integer> indents, ZscriptDataModel.GenericField field, CommandPrintSettings settings, int value) {
        StringBuilder builder;
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

    private void explainField(List<String> lines, List<Integer> indents, ZscriptDataModel.GenericField field, boolean[] doneFields,
            ZscriptFieldSet fieldSet, CommandPrintSettings settings) {
        indents.add(1);
        StringBuilder builder = new StringBuilder();

        char key = field.getKey();
        builder.append(upperFirst(field.getName()));
        builder.append(": ");
        if (field.getTypeDefinition() instanceof ZscriptDataModel.BigfieldTypeDefinition) {
            explainBigField(lines, indents, field, fieldSet, settings, builder);
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
                explainBitField(lines, indents, field, settings, value);
            }
        }
    }

    private void explainStatus(ZscriptModel model, CommandPrintSettings settings, ZscriptDataModel.ResponseFieldModel field, List<Integer> indents, boolean[] doneFields,
            ZscriptFieldSet fieldSet, List<String> lines, ZscriptDataModel.CommandModel command) {
        StringBuilder builder;
        indents.add(1);
        builder = new StringBuilder();

        char key = field.getKey();
        builder.append(upperFirst(field.getName()));
        builder.append(": ");
        doneFields[key - 'A'] = true;
        int value = fieldSet.getFieldValue(key);
        if (value == -1) {
            if (field.isRequired()) {
                builder.append("Is required but missing");
            } else {
                builder.append("Not present");
            }
        } else {
            builder.append("0x");
            ZscriptFieldSet.hexString(builder, value, 1);
            builder.append(" (");
            Optional<ZscriptDataModel.StatusModel> optStatus = model.getStatus(value);
            if (optStatus.isPresent()) {
                builder.append(upperFirst(optStatus.get().getCode()));
            } else {
                builder.append("Unknown");
            }
            builder.append(")");

        }
        lines.add(builder.toString());
        if (settings.getVerbosity().compareTo(VerbositySetting.DESCRIPTIONS) >= 0) {
            indents.add(3);
            boolean hasSpecificDesc = false;
            if (value != -1) {
                Optional<ZscriptDataModel.StatusModel> optStatus = model.getStatus(value);
                if (optStatus.isPresent()) {
                    for (ZscriptDataModel.StatusModel specific : command.getStatus()) {
                        if (specific.getCode().equals(optStatus.get().getCode())) {
                            hasSpecificDesc = true;
                            lines.add(toSentence(specific.getMeaning()));
                        }
                    }
                    if (!hasSpecificDesc) {
                        lines.add(toSentence(optStatus.get().getMeaning()));
                        hasSpecificDesc = true;
                    }
                }
            }
            if (!hasSpecificDesc) {
                lines.add(toSentence(field.getDescription()));
            }
        }
    }

    private String explainUnknownFields(CommandPrintSettings settings, ZscriptFieldSet fieldSet, List<String> lines, List<Integer> indents,
            boolean[] doneFields) {
        StringBuilder builder;
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

    public String explainCommand(Command target, ZscriptModel model, CommandPrintSettings settings) {
        ZscriptFieldSet fieldSet = target.getFields();
        List<String>    lines    = new ArrayList<>();
        List<Integer>   indents  = new ArrayList<>();

        int           commandValue = fieldSet.getFieldValue('Z');
        StringBuilder builder      = new StringBuilder();
        indents.add(0);
        if (fieldSet.isEmpty()) {
            builder.append("Empty Command");
            lines.add(builder.toString());
            return settings.lineWrap(lines, indents);
        }
        builder.append(target);
        if (settings.getVerbosity().compareTo(VerbositySetting.MINIMAL) <= 0) {
            lines.add(builder.toString());
            return settings.lineWrap(lines, indents);
        }
        builder.append(": ");
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
        builder.append(upperFirst(command.getModule().getName()));
        builder.append(' ');
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
            explainField(lines, indents, field, doneFields, fieldSet, settings);
        }
        return explainUnknownFields(settings, fieldSet, lines, indents, doneFields);
    }

    public String explainResponse(Command source, Response target, ZscriptModel model, CommandPrintSettings settings) {
        int             commandValue = source.getFields().getFieldValue('Z');
        ZscriptFieldSet fieldSet     = target.getFields();
        List<String>    lines        = new ArrayList<>();
        List<Integer>   indents      = new ArrayList<>();

        StringBuilder builder = new StringBuilder();
        indents.add(0);
        if (fieldSet.isEmpty()) {
            builder.append("Empty Response");
            lines.add(builder.toString());
            return settings.lineWrap(lines, indents);
        }
        builder.append(target);
        if (settings.getVerbosity().compareTo(VerbositySetting.MINIMAL) <= 0) {
            lines.add(builder.toString());
            return settings.lineWrap(lines, indents);
        }
        builder.append(": ");
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
        builder.append(upperFirst(command.getModule().getName()));
        builder.append(' ');
        builder.append(upperFirst(command.getCommandName()));
        builder.append(" Response");
        if (settings.getVerbosity().compareTo(VerbositySetting.NAME) <= 0) {
            lines.add(builder.toString());
            return settings.lineWrap(lines, indents);
        }
        lines.add(builder.toString());
        boolean[] doneFields = new boolean[26];
        for (ZscriptDataModel.ResponseFieldModel field : command.getResponseFields()) {
            if (field.getKey() == 'S') {
                explainStatus(model, settings, field, indents, doneFields, fieldSet, lines, command);
                break;
            }
        }
        for (ZscriptDataModel.ResponseFieldModel field : command.getResponseFields()) {
            if (field.getKey() != 'S') {
                explainField(lines, indents, field, doneFields, fieldSet, settings);
            }
        }
        return explainUnknownFields(settings, fieldSet, lines, indents, doneFields);
    }

    private void addLanes(StringBuilder output, boolean[] presentLanes, int highlightLane, int start, int end, boolean leftAlign, String outputSettings) {
        output.append(outputSettings);
        for (int i = start; i < end; i++) {
            if (presentLanes[i]) {
                if (highlightLane == i) {
                    output.append(actualPath);
                }
                if (leftAlign) {
                    output.append("|  ");
                } else {
                    output.append("  |");
                }
                output.append(outputSettings);
                if (highlightLane == i) {
                    output.append(ANSI_RESET);
                    output.append(outputSettings);
                }
            } else {
                output.append("   ");
            }
        }
        output.append(ANSI_RESET);
    }

    public String graph(Map<Command, CommandGraphElement> commands, List<CommandGraphElement> elements,
            CommandDepth maxDepth, List<Command> toHighlight, boolean skipImpossiblePaths) {
        StringBuilder output        = new StringBuilder();
        boolean[]     hasLane       = new boolean[maxDepth.getDepth() + 1];
        int           highlightLane = -1;
        hasLane[0] = true;
        int highlightIndex = 0;
        for (CommandGrapher.CommandGraphElement element : elements) {
            // Set up variables
            int elementDepth = element.getDepth().getDepth();

            Command successCmd = element.getCommand().getEndLink().getOnSuccess();
            if (successCmd == null || skipImpossiblePaths && !element.getCommand().canSucceed()) {
                successCmd = null;
            } else if (skipImpossiblePaths && !elements.contains(commands.get(successCmd))) {
                successCmd = successCmd.canSucceed() ? successCmd.getEndLink().getOnSuccess() : successCmd.getEndLink().getOnFail();
                while (successCmd != null && !elements.contains(commands.get(successCmd))) {
                    successCmd = successCmd.canSucceed() ? successCmd.getEndLink().getOnSuccess() : successCmd.getEndLink().getOnFail();
                }
            }
            CommandGrapher.CommandDepth successDepth;

            int successDepthI;
            if (successCmd == null) {
                successDepth = null;
                successDepthI = -1;
            } else {
                successDepth = commands.get(successCmd).getDepth();
                successDepthI = successDepth.getDepth();
            }
            Command failCmd = element.getCommand().getEndLink().getOnFail();

            if (failCmd == null || skipImpossiblePaths && !element.getCommand().canFail()) {
                failCmd = null;
            } else if (skipImpossiblePaths && !elements.contains(commands.get(failCmd))) {
                failCmd = failCmd.canSucceed() ? failCmd.getEndLink().getOnSuccess() : failCmd.getEndLink().getOnFail();
                while (failCmd != null && !elements.contains(commands.get(failCmd))) {
                    failCmd = failCmd.canSucceed() ? failCmd.getEndLink().getOnSuccess() : failCmd.getEndLink().getOnFail();
                }
            }
            CommandGrapher.CommandDepth failDepth;

            int failDepthI;
            if (failCmd == null) {
                failDepth = null;
                failDepthI = -1;
            } else {
                failDepth = commands.get(failCmd).getDepth();
                failDepthI = failDepth.getDepth();
            }
            if (successDepthI != elementDepth) {
                hasLane[elementDepth] = false;
            }
            boolean highlightSuccess = false;
            boolean highlightFail    = false;
            boolean highlightElement = false;

            if (!toHighlight.isEmpty() && element.getCommand() == toHighlight.get(highlightIndex)) {
                highlightElement = true;
                if (highlightIndex + 1 < toHighlight.size()) {
                    if (successCmd == toHighlight.get(highlightIndex + 1)) {
                        highlightIndex++;
                        highlightLane = -1;
                        highlightSuccess = true;
                    } else if (failCmd == toHighlight.get(highlightIndex + 1)) {
                        highlightIndex++;
                        highlightLane = -1;
                        highlightFail = true;
                    }
                } else {
                    highlightLane = -1;
                }
            }

            // Begin the actual outputting
            addLanes(output, hasLane, highlightLane, 0, elementDepth, true, ANSI_GREEN);
            if (element.getCommand().isEmpty()) {
                output.append('|');
            } else {
                output.append('O');
            }
            addLanes(output, hasLane, highlightLane, elementDepth + 1, hasLane.length, false, ANSI_RED);
            if (highlightElement) {
                output.append(actualPath);
                output.append(" * ");
                output.append(ANSI_RESET);
            } else {
                output.append("   ");
            }
            if (!element.getCommand().isEmpty()) {
                output.append("Command description");
            }
            output.append("\n");

            // first moving round...
            if (successDepth == null || successDepthI == elementDepth) {
                addLanes(output, hasLane, highlightLane, 0, elementDepth, true, ANSI_GREEN);
                if (successDepth != null) {
                    if (highlightSuccess) {
                        output.append(actualPath);
                    } else {
                        output.append(ANSI_GREEN);
                    }
                    output.append('|');
                    output.append(ANSI_RESET);
                } else {
                    output.append(' ');
                }
            } else {
                addLanes(output, hasLane, highlightLane, 0, successDepthI, true, ANSI_GREEN);
                if (hasLane[successDepthI]) {
                    if (highlightLane == successDepthI) {
                        output.append(actualPath);
                    } else {
                        output.append(ANSI_GREEN);
                    }
                    output.append('|');
                    output.append(ANSI_RESET);
                } else {
                    output.append(' ');
                }
                if (highlightSuccess) {
                    output.append(actualPath);
                } else {
                    output.append(ANSI_GREEN);
                }
                output.append(' ');
                output.append("___".repeat(elementDepth - successDepthI - 1));
                output.append("/ ");
            }
            output.append(ANSI_RESET);
            if (failDepth == null) {
                addLanes(output, hasLane, highlightLane, elementDepth + 1, hasLane.length, false, ANSI_RED);
            } else {
                if (highlightFail) {
                    output.append(actualPath);
                } else {
                    output.append(ANSI_RED);
                }
                output.append("\\");
                output.append("___".repeat(failDepthI - elementDepth - 1));
                output.append(' ');
                if (hasLane[failDepthI]) {
                    if (highlightLane == failDepthI) {
                        output.append(actualPath);
                    } else {
                        output.append(ANSI_RED);
                    }
                    output.append('|');
                    output.append(ANSI_RESET);
                } else {
                    output.append(' ');
                }
                addLanes(output, hasLane, highlightLane, failDepthI + 1, hasLane.length, false, ANSI_RED);
            }
            output.append(ANSI_RESET);
            if (highlightElement) {
                output.append("     ");
                output.append("Response details");
            }

            output.append("\n");
            // next moving round
            if (successDepth == null || successDepthI == elementDepth) {
                addLanes(output, hasLane, highlightLane, 0, elementDepth, true, ANSI_GREEN);
                if (successDepth != null) {
                    if (highlightSuccess) {
                        output.append(actualPath);
                    } else {
                        output.append(ANSI_GREEN);
                    }
                    output.append('|');
                    output.append(ANSI_RESET);
                } else {
                    output.append(' ');
                }
            } else {
                addLanes(output, hasLane, highlightLane, 0, successDepthI, true, ANSI_GREEN);
                if (hasLane[successDepthI]) {
                    if (highlightLane == successDepthI) {
                        output.append(actualPath);
                    } else {
                        output.append(ANSI_GREEN);
                    }
                    output.append('|');
                    output.append(ANSI_RESET);
                } else {
                    output.append(' ');
                }
                if (highlightSuccess) {
                    output.append(actualPath);
                } else {
                    output.append(ANSI_GREEN);
                }
                output.append('/');
                output.append("   ".repeat(elementDepth - successDepthI - 1));
                output.append("  ");
                output.append(ANSI_RESET);
            }
            output.append(ANSI_RED);
            if (failDepth == null) {
                addLanes(output, hasLane, highlightLane, elementDepth + 1, hasLane.length, false, ANSI_RED);
            } else {
                if (highlightFail) {
                    output.append(actualPath);
                } else {
                    output.append(ANSI_RED);
                }
                output.append(" ");
                output.append("   ".repeat(failDepthI - elementDepth - 1));
                output.append('\\');
                output.append(ANSI_RESET);
                if (hasLane[failDepthI]) {
                    if (highlightLane == failDepthI) {
                        output.append(actualPath);
                    } else {
                        output.append(ANSI_RED);
                    }
                    output.append('|');
                    output.append(ANSI_RESET);
                } else {
                    output.append(' ');
                }
                addLanes(output, hasLane, highlightLane, failDepthI + 1, hasLane.length, false, ANSI_RED);
            }
            output.append(ANSI_RESET);
            output.append("\n");
            if (highlightSuccess) {
                highlightLane = successDepthI;
            } else if (highlightFail) {
                highlightLane = failDepthI;
            }

            if (failDepth != null) {
                hasLane[failDepthI] = true;
            }
        }
        return output.toString();

    }
}
