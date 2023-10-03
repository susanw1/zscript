package net.zscript.javaclient.core;

import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.Optional;

import net.zscript.ascii.AnsiCharacterStylePrinter;
import net.zscript.ascii.AsciiFrame;
import net.zscript.ascii.CharacterStyle;
import net.zscript.ascii.TextBox;
import net.zscript.ascii.TextCanvas;
import net.zscript.ascii.TextColor;
import net.zscript.model.ZscriptModel;
import net.zscript.model.components.Zchars;
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
        private final String           indentString;
        private final VerbositySetting verbosity;

        public CommandPrintSettings(String indentString, VerbositySetting verbosity) {
            this.indentString = indentString;
            this.verbosity = verbosity;
        }

        public VerbositySetting getVerbosity() {
            return verbosity;
        }

    }

    private String toSentence(String desc) {
        return desc.substring(0, 1).toUpperCase() + desc.substring(1) + '.';
    }

    private String upperFirst(String name) {
        return name.substring(0, 1).toUpperCase() + name.substring(1);
    }

    private void explainBigField(TextBox box, ZscriptDataModel.GenericField field, ZscriptFieldSet fieldSet,
            CommandPrintSettings settings) {
        byte[] bigFields = fieldSet.getBigFieldTotal();
        if (bigFields.length == 0) {
            box.append("No data");
        } else {
            if (field.getTypeDefinition() instanceof ZscriptDataModel.TextTypeDefinition) {
                box.append('"');
                box.append(StandardCharsets.UTF_8.decode(ByteBuffer.wrap(bigFields)));
                box.append('"');
            } else {
                box.append("(Hex) ");
                for (byte b : bigFields) {
                    box.append((char) ZscriptFieldSet.hexchar((byte) (b >> 4)));
                    box.append((char) ZscriptFieldSet.hexchar((byte) (b & 0xF)));
                    box.append(" ");
                }
            }
        }

        if (settings.getVerbosity().compareTo(VerbositySetting.DESCRIPTIONS) >= 0) {
            box.startNewLine(3);
            box.append(toSentence(field.getDescription()));
        }
    }

    private void explainBitField(TextBox box, ZscriptDataModel.GenericField field, CommandPrintSettings settings, int value) {
        if (field.getTypeDefinition() instanceof ZscriptDataModel.BitsetTypeDefinition) {
            List<ZscriptDataModel.BitsetTypeDefinition.Bit> bits = ((ZscriptDataModel.BitsetTypeDefinition) field.getTypeDefinition()).getBits();
            int                                             i    = 0;
            for (ZscriptDataModel.BitsetTypeDefinition.Bit bit : bits) {
                box.startNewLine(2);
                box.append(upperFirst(bit.getName()));
                if (value != -1) {
                    box.append(": ");
                    if ((value & (1 << i)) != 0) {
                        box.append("True");
                    } else {
                        box.append("False");
                    }
                }
                if (settings.getVerbosity().compareTo(VerbositySetting.DESCRIPTIONS) >= 0) {
                    box.startNewLine(4);
                    box.append(toSentence(bit.getDescription()));
                }
                i++;
            }
        }
    }

    private void explainField(TextBox box, ZscriptDataModel.GenericField field, boolean[] doneFields,
            ZscriptFieldSet fieldSet, CommandPrintSettings settings) {
        box.startNewLine(1);

        char key = field.getKey();
        box.append(upperFirst(field.getName()));
        box.append(" (");
        box.append(key);
        box.append("): ");
        if (field.getTypeDefinition() instanceof ZscriptDataModel.BigfieldTypeDefinition) {
            explainBigField(box, field, fieldSet, settings);
        } else {
            doneFields[key - 'A'] = true;
            int value = fieldSet.getFieldValue(key);
            if (value == -1) {
                if (field.isRequired()) {
                    box.setStyle(new CharacterStyle(TextColor.RED, TextColor.DEFAULT, true));
                    box.append("Error: Is required but missing");
                    box.setStyle(CharacterStyle.standardStyle());
                } else {
                    box.append("Not present");
                }
            } else {
                if (field.getTypeDefinition() instanceof ZscriptDataModel.FlagTypeDefinition) {
                    box.append("Present");
                } else {
                    box.append("0x");
                    box.appendHex(value, 1);
                }
                if (field.getTypeDefinition() instanceof ZscriptDataModel.AnyTypeDefinition
                        || field.getTypeDefinition() instanceof ZscriptDataModel.NumberTypeDefinition) {
                    box.append(" (");
                    box.append(value);
                    box.append(")");
                } else if (field.getTypeDefinition() instanceof ZscriptDataModel.EnumTypeDefinition) {
                    box.append(" (");
                    List<String> values = ((ZscriptDataModel.EnumTypeDefinition) field.getTypeDefinition()).getValues();
                    if (value >= values.size()) {
                        box.append("Out of range");
                    } else {
                        box.append(upperFirst(values.get(value)));
                    }
                    box.append(")");
                }
            }
            if (settings.getVerbosity().compareTo(VerbositySetting.DESCRIPTIONS) >= 0) {
                box.startNewLine(3);
                box.append(toSentence(field.getDescription()));
            }
            if (settings.getVerbosity().compareTo(VerbositySetting.BITFIELDS) >= 0) {
                explainBitField(box, field, settings, value);
            }
        }
    }

    private void explainStatus(ZscriptModel model, CommandPrintSettings settings, ZscriptDataModel.ResponseFieldModel field, TextBox box, boolean[] doneFields,
            ZscriptFieldSet fieldSet, ZscriptDataModel.CommandModel command) {
        box.startNewLine(1);

        char key = field.getKey();
        box.append(upperFirst(field.getName()));
        box.append(" (");
        box.append(key);
        box.append("): ");
        doneFields[key - 'A'] = true;
        int value = fieldSet.getFieldValue(key);
        if (value == -1) {
            if (field.isRequired()) {
                box.setStyle(new CharacterStyle(TextColor.RED, TextColor.DEFAULT, true));
                box.append("Error: Is required but missing");
                box.setStyle(CharacterStyle.standardStyle());
            } else {
                box.append("Not present");
            }
        } else {
            box.append("0x");
            box.appendHex(value, 1);
            box.append(" (");
            Optional<ZscriptDataModel.StatusModel> optStatus = model.getStatus(value);
            if (optStatus.isPresent()) {
                box.append(upperFirst(optStatus.get().getCode()));
            } else {
                box.append("Unknown");
            }
            box.append(")");

        }
        if (settings.getVerbosity().compareTo(VerbositySetting.DESCRIPTIONS) >= 0) {
            box.startNewLine(3);
            boolean hasSpecificDesc = false;
            if (value != -1) {
                Optional<ZscriptDataModel.StatusModel> optStatus = model.getStatus(value);
                if (optStatus.isPresent()) {
                    for (ZscriptDataModel.StatusModel specific : command.getStatus()) {
                        if (specific.getCode().equals(optStatus.get().getCode())) {
                            hasSpecificDesc = true;
                            box.append(toSentence(specific.getMeaning()));
                        }
                    }
                    if (!hasSpecificDesc) {
                        box.append(toSentence(optStatus.get().getMeaning()));
                        hasSpecificDesc = true;
                    }
                }
            }
            if (!hasSpecificDesc) {
                box.append(toSentence(field.getDescription()));
            }
        }
    }

    private AsciiFrame explainUnknownFields(TextBox box, CommandPrintSettings settings, ZscriptFieldSet fieldSet,
            boolean[] doneFields) {
        for (int i = 0; i < 26; i++) {
            if (!doneFields[i] && fieldSet.getFieldValue((byte) ('A' + i)) != -1) {
                box.startNewLine(1);
                box.append("Unknown Field ");
                box.append((char) ('A' + i));
                box.append(" with value: ");
                box.append("0x");
                box.appendHex(fieldSet.getFieldValue((byte) ('A' + i)), 1);
            }
        }
        return box;
    }

    public AsciiFrame explainCommand(Command target, ZscriptModel model, CommandPrintSettings settings) {
        TextBox box = new TextBox(settings.indentString);
        box.setIndentOnWrap(1);

        ZscriptFieldSet fieldSet = target.getFields();

        int commandValue = fieldSet.getFieldValue('Z');
        box.startNewLine(0);
        if (fieldSet.isEmpty()) {
            box.append("Empty Command");
            return box;
        }
        ZscriptDataModel.CommandModel command = null;
        if (settings.getVerbosity().compareTo(VerbositySetting.MINIMAL) > 0) {
            if (commandValue == -1) {
                box.append("No command field - Z expected");
            } else {
                Optional<ZscriptDataModel.CommandModel> commandOpt = model.getCommand(commandValue);
                if (commandOpt.isEmpty()) {
                    box.append("Command not recognised");
                } else {
                    command = commandOpt.get();
                    box.append(upperFirst(command.getModule().getName()));
                    box.append(' ');
                    box.append(upperFirst(command.getCommandName()));
                }
            }
            box.append(": ");

        }
        box.append(target);
        if (command == null || settings.getVerbosity().compareTo(VerbositySetting.NAME) <= 0) {
            return box;
        }

        if (settings.getVerbosity().compareTo(VerbositySetting.DESCRIPTIONS) >= 0) {
            box.startNewLine(2);
            box.append(toSentence(command.getDescription()));
        }

        boolean[] doneFields = new boolean[26];
        for (ZscriptDataModel.RequestFieldModel field : command.getRequestFields()) {
            if (field.getKey() == Zchars.Z_CMD) {
                doneFields[field.getKey() - 'A'] = true;
                continue;
            }
            explainField(box, field, doneFields, fieldSet, settings);
        }
        return explainUnknownFields(box, settings, fieldSet, doneFields);
    }

    public AsciiFrame explainResponse(Command source, Response target, ZscriptModel model, CommandPrintSettings settings) {
        int             commandValue = source.getFields().getFieldValue('Z');
        ZscriptFieldSet fieldSet     = target.getFields();

        TextBox box = new TextBox(settings.indentString);
        box.setIndentOnWrap(1);

        box.startNewLine(0);
        if (fieldSet.isEmpty()) {
            box.append("Empty Response");
            return box;
        }
        ZscriptDataModel.CommandModel command = null;
        if (settings.getVerbosity().compareTo(VerbositySetting.MINIMAL) > 0) {
            if (commandValue == -1) {
                box.append("Response to unrecognised command");
            } else {
                Optional<ZscriptDataModel.CommandModel> commandOpt = model.getCommand(commandValue);
                if (commandOpt.isEmpty()) {
                    box.append("Response to unrecognised command");
                } else {
                    command = commandOpt.get();
                    box.append(upperFirst(command.getModule().getName()));
                    box.append(' ');
                    box.append(upperFirst(command.getCommandName()));
                    box.append(" Response");
                }
            }
            box.append(": ");
        }
        box.append(target);
        if (command == null || settings.getVerbosity().compareTo(VerbositySetting.NAME) <= 0) {
            return box;
        }
        boolean[] doneFields = new boolean[26];
        for (ZscriptDataModel.ResponseFieldModel field : command.getResponseFields()) {
            if (field.getKey() == 'S') {
                explainStatus(model, settings, field, box, doneFields, fieldSet, command);
                break;
            }
        }
        for (ZscriptDataModel.ResponseFieldModel field : command.getResponseFields()) {
            if (field.getKey() != 'S') {
                explainField(box, field, doneFields, fieldSet, settings);
            }
        }
        return explainUnknownFields(box, settings, fieldSet, doneFields);
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

    public AsciiFrame graph(ZscriptModel model, Map<Command, CommandGraphElement> commands, List<CommandGraphElement> elements,
            CommandDepth maxDepth, List<Command> toHighlight, List<Response> responses, CommandGraph.GraphPrintSettings settings) {
        return new CommandGraph(model, commands, elements, maxDepth, toHighlight, responses, settings);
    }

    public String graph2(Map<Command, CommandGraphElement> commands, List<CommandGraphElement> elements,
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
