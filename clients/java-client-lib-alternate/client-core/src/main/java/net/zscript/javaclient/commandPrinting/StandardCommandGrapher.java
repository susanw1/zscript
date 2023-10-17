package net.zscript.javaclient.commandPrinting;

import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;
import java.util.List;
import java.util.ArrayDeque;
import java.util.ArrayList;
import java.util.Deque;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.Optional;

import net.zscript.ascii.AsciiFrame;
import net.zscript.ascii.CharacterStyle;
import net.zscript.ascii.TextBox;
import net.zscript.ascii.TextColor;
import net.zscript.javaclient.commandPaths.Command;
import net.zscript.javaclient.commandPaths.CommandExecutionPath;
import net.zscript.javaclient.commandPaths.MatchedCommandResponse;
import net.zscript.javaclient.commandPaths.Response;
import net.zscript.javaclient.commandPaths.ResponseExecutionPath;
import net.zscript.javaclient.commandPaths.ZscriptFieldSet;
import net.zscript.javaclient.ZscriptByteString;
import net.zscript.model.ZscriptModel;
import net.zscript.model.components.Zchars;
import net.zscript.model.datamodel.IntrinsicsDataModel.StatusModel;
import net.zscript.model.datamodel.ZscriptDataModel;

public class StandardCommandGrapher implements CommandGrapher<AsciiFrame, StandardCommandGrapher.CommandPrintSettings, CommandGraph.GraphPrintSettings> {

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

    private void explainBigField(TextBox box, ZscriptDataModel.GenericField field, ZscriptFieldSet fieldSet, CommandPrintSettings settings) {
        byte[] bigFields = fieldSet.getBigFieldData();
        if (bigFields.length == 0) {
            box.append("No data");
        } else {
            if (field.getTypeDefinition() instanceof ZscriptDataModel.TextTypeDefinition) {
                box.append('"');
                box.append(StandardCharsets.UTF_8.decode(ByteBuffer.wrap(bigFields)));
                box.append('"');
            } else {
                box.append("(Hex) ");
                ZscriptByteString.ZscriptByteStringBuilder builder = ZscriptByteString.builder();
                for (byte b : bigFields) {
                    builder.appendHexPair(b);
                    builder.appendByte(' ');
                }
                box.append(builder.asString());
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

    private void explainField(TextBox box, ZscriptDataModel.GenericField field, boolean[] doneFields, ZscriptFieldSet fieldSet, CommandPrintSettings settings) {
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
                if (field.getTypeDefinition() instanceof ZscriptDataModel.AnyTypeDefinition || field.getTypeDefinition() instanceof ZscriptDataModel.NumberTypeDefinition) {
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
            Optional<StatusModel> optStatus = model.getStatus(value);
            if (optStatus.isPresent()) {
                box.append(upperFirst(optStatus.get().getName()));
            } else {
                box.append("Unknown");
            }
            box.append(")");

        }
        if (settings.getVerbosity().compareTo(VerbositySetting.DESCRIPTIONS) >= 0) {
            box.startNewLine(3);
            boolean hasSpecificDesc = false;
            if (value != -1) {
                Optional<StatusModel> optStatus = model.getStatus(value);
                if (optStatus.isPresent()) {
                    for (ZscriptDataModel.CommandStatusModel specific : command.getStatus()) {
                        if (specific.getName().equals(optStatus.get().getName())) {
                            hasSpecificDesc = true;
                            box.append(toSentence(specific.getDescription()));
                        }
                    }
                    if (!hasSpecificDesc) {
                        box.append(toSentence(optStatus.get().getDescription()));
                        hasSpecificDesc = true;
                    }
                }
            }
            if (!hasSpecificDesc) {
                box.append(toSentence(field.getDescription()));
            }
        }
    }

    private void explainUnknownFields(TextBox box, CommandPrintSettings settings, ZscriptFieldSet fieldSet, boolean[] doneFields) {
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
        explainUnknownFields(box, settings, fieldSet, doneFields);
        return box;
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
        explainUnknownFields(box, settings, fieldSet, doneFields);
        return box;
    }

    public AsciiFrame graph(CommandExecutionPath path, CommandGraph.GraphPrintSettings settings) {
        return graph(path, null, settings);
    }

    @Override
    public AsciiFrame graph(CommandExecutionPath path, ResponseExecutionPath resps, CommandGraph.GraphPrintSettings settings) {
        Command firstCommand = path.getFirstCommand();

        boolean skipImpossiblePaths = settings.skipImpossiblePaths();

        CommandGrapher.CommandDepth startDepth = new CommandGrapher.CommandDepth(0);

        Map<Command, CommandGrapher.CommandGraphElement> commands = new HashMap<>();

        Deque<CommandGraphElement>                openedTrees  = new ArrayDeque<>();
        Deque<CommandGrapher.CommandGraphElement> workingTrees = new ArrayDeque<>();
        List<CommandGrapher.CommandGraphElement>  elements     = new ArrayList<>();
        if (firstCommand != null) {
            CommandGrapher.CommandGraphElement start = new CommandGrapher.CommandGraphElement(firstCommand, startDepth);
            commands.put(firstCommand, start);
            workingTrees.push(start);
        }
        CommandGrapher.CommandDepth maxDepth = new CommandGrapher.CommandDepth(0);
        while (!workingTrees.isEmpty()) {
            CommandGrapher.CommandGraphElement current = workingTrees.peek();
            Command             cmd   = current.getCommand();

            CommandGrapher.CommandGraphElement latestOpenTree = openedTrees.peek();
            if (latestOpenTree != null && cmd.getOnFail() != latestOpenTree.getCommand()) {
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
                    tmp = tmp.getOnSuccess();
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
            if (cmd.getOnFail() != null && (!skipImpossiblePaths || current.getCommand().canFail()) && (latestOpenTree == null
                    || cmd.getOnFail() != latestOpenTree.getCommand())) {
                // the command opens a new open tree
                CommandGrapher.CommandGraphElement opened = new CommandGrapher.CommandGraphElement(cmd.getOnFail(), new CommandGrapher.CommandDepth(current.getDepth()));
                maxDepth.depthGreaterThan(current.getDepth());
                commands.put(opened.getCommand(), opened);
                openedTrees.push(opened);
                latestOpenTree = opened;
            }

            elements.add(workingTrees.pop()); // only put elements into the list when we're done processing them
            if (cmd.getOnSuccess() != null && (!skipImpossiblePaths || current.getCommand().canSucceed())) {
                if (workingTrees.isEmpty() || cmd.getOnSuccess() != workingTrees.peek().getCommand()) {
                    // check we're not on a close parent, dropping out of failure
                    CommandGrapher.CommandGraphElement next = new CommandGrapher.CommandGraphElement(cmd.getOnSuccess(), current.getDepth());
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
        List<MatchedCommandResponse> toHighlight = List.of();
        if (resps != null) {
            toHighlight = path.compareResponses(resps);
        }
        if (skipImpossiblePaths) {
            elements = skipEmpty(commands, elements);
            toHighlight = new ArrayList<>(toHighlight);
            for (Iterator<MatchedCommandResponse> iter = toHighlight.iterator(); iter.hasNext(); ) {
                if (!elements.contains(commands.get(iter.next().getCommand()))) {
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
            maxDepth.setDepth(maxDepth.getDepth() - offset);
        }
        return new CommandGraph(path.getModel(), commands, elements, maxDepth, toHighlight, settings);
    }

    private List<CommandGrapher.CommandGraphElement> skipEmpty(Map<Command, CommandGrapher.CommandGraphElement> commands, List<CommandGrapher.CommandGraphElement> elements) {
        List<CommandGrapher.CommandGraphElement> compactedElements = new ArrayList<>();
        for (CommandGrapher.CommandGraphElement element : elements) {
            if (element.getCommand().isEmpty()) {
                CommandGrapher.CommandGraphElement tmp = element;
                while (tmp.getCommand().isEmpty() && tmp.getCommand().getOnSuccess() != null) {
                    tmp = commands.get(tmp.getCommand().getOnSuccess());
                }
                if ((!tmp.getCommand().isEmpty() || tmp.getCommand().getOnSuccess() != null) && element.getDepth().getDepth() != commands.get(
                        element.getCommand().getOnSuccess()).getDepth().getDepth()) {
                    compactedElements.add(element);
                }
            } else {
                compactedElements.add(element);
            }
        }
        return compactedElements;
    }

    @Override
    public CommandGraph.GraphPrintSettings getDefaultGraphSettings() {
        return getDefaultGraphSettings(getDefaultExplainerSettings(), false);
    }

    @Override
    public CommandGraph.GraphPrintSettings getDefaultGraphSettings(boolean skipImpossiblePaths) {
        return getDefaultGraphSettings(getDefaultExplainerSettings(), skipImpossiblePaths);
    }

    @Override
    public CommandGraph.GraphPrintSettings getDefaultGraphSettings(CommandPrintSettings explainerSettings, boolean skipImpossiblePaths) {
        return new CommandGraph.GraphPrintSettings(explainerSettings, skipImpossiblePaths, 2, 80);
    }

    @Override
    public CommandPrintSettings getDefaultExplainerSettings() {
        return getDefaultExplainerSettings(VerbositySetting.FIELDS);
    }

    @Override
    public CommandPrintSettings getDefaultExplainerSettings(VerbositySetting v) {
        return new CommandPrintSettings("  ", v);
    }

}
