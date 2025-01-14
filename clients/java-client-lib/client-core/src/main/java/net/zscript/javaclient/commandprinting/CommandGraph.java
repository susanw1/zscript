package net.zscript.javaclient.commandprinting;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

import net.zscript.ascii.AsciiFrame;
import net.zscript.ascii.CharacterStyle;
import net.zscript.ascii.TextCanvas;
import net.zscript.ascii.TextColor;
import net.zscript.javaclient.commandpaths.Command;
import net.zscript.javaclient.commandpaths.MatchedCommandResponse;
import net.zscript.model.ZscriptModel;

public class CommandGraph extends TextCanvas {

    public static class GraphPrintSettings {
        private final StandardCommandGrapher.CommandPrintSettings settings;

        private final boolean skipImpossiblePaths;
        private final int     spacing;
        private final int     width;

        public GraphPrintSettings(StandardCommandGrapher.CommandPrintSettings settings, boolean skipImpossiblePaths, int spacing, int width) {
            this.settings = settings;
            this.skipImpossiblePaths = skipImpossiblePaths;
            this.spacing = spacing;
            this.width = width;
        }

        public boolean skipImpossiblePaths() {
            return skipImpossiblePaths;
        }
    }

    private final ZscriptModel                                     model;
    private final Map<Command, CommandGrapher.CommandGraphElement> commands;
    private final List<CommandGrapher.CommandGraphElement>         elements;
    private final CommandGrapher.CommandDepth                      maxDepth;
    private final List<MatchedCommandResponse>                     toHighlight;
    private       StandardCommandGrapher.CommandPrintSettings      settings;
    private       int                                              spacing;
    private       boolean                                          skipImpossiblePaths;

    public static GraphPrintSettings getDefaultSettings() {
        return new GraphPrintSettings(new StandardCommandGrapher.CommandPrintSettings("  ", VerbositySetting.BITFIELDS), false, 2, 80);
    }

    public CommandGraph(ZscriptModel model, Map<Command, CommandGrapher.CommandGraphElement> commands,
            List<CommandGrapher.CommandGraphElement> elements, CommandGrapher.CommandDepth maxDepth, List<MatchedCommandResponse> toHighlight,
            GraphPrintSettings settings) {
        super(settings.width, 2);
        this.model = model;
        this.commands = commands;
        this.elements = elements;
        this.maxDepth = maxDepth;
        this.toHighlight = toHighlight;
        this.skipImpossiblePaths = settings.skipImpossiblePaths;
        this.settings = settings.settings;
        this.spacing = settings.spacing + 1;
        recalculate(settings.width);
    }

    public void setSettings(GraphPrintSettings newSettings) {
        this.spacing = newSettings.spacing;
        this.skipImpossiblePaths = newSettings.skipImpossiblePaths;
        this.settings = newSettings.settings;
        recalculate(newSettings.width);
    }

    @Override
    public boolean setWidth(int width) {
        if (width < maxDepth.getDepth() * spacing + 1 + 3 + 10) {
            return false;
        }
        recalculate(width);
        return true;
    }

    private void recalculate(int width) {
        LineDrawingStrategy strategy = new LineDrawingStrategy(false, true, false, false);

        CharacterStyle successStyle   = new CharacterStyle(TextColor.GREEN, TextColor.DEFAULT, false);
        CharacterStyle failStyle      = new CharacterStyle(TextColor.RED, TextColor.DEFAULT, false);
        CharacterStyle highlightStyle = new CharacterStyle(TextColor.CYAN, TextColor.DEFAULT, true);
        CharacterStyle defaultStyle   = CharacterStyle.standardStyle();

        StandardCommandGrapher           g                     = new StandardCommandGrapher();
        Set<CommandGrapher.CommandDepth> currentSuccessDepths  = new HashSet<>();
        Set<CommandGrapher.CommandDepth> currentFailDepths     = new HashSet<>();
        CommandGrapher.CommandDepth      currentHighlightDepth = null;

        int highlightIndex = 0;
        int currentVert    = 0;
        int textHorizontal = maxDepth.getDepth() * spacing + 1 + 3;

        List<CanvasElement> toApply = new ArrayList<>();

        for (CommandGrapher.CommandGraphElement element : elements) {
            int nextVert = currentVert;

            CommandGrapher.CommandDepth oldHLDepth = currentHighlightDepth;

            Command cmd = element.getCommand();
            if (!cmd.isEmpty()) {
                AsciiFrame cmdExp = g.explainCommand(cmd, model, settings);
                cmdExp.setWidth(width - textHorizontal);
                toApply.add(new AsciiFrameElement(cmdExp, textHorizontal, currentVert));
                nextVert += cmdExp.getHeight() + 1;
                if (highlightIndex < toHighlight.size() && toHighlight.get(highlightIndex).getCommand() == cmd && !toHighlight.isEmpty()) {
                    AsciiFrame respExp = g.explainResponse(cmd, toHighlight.get(highlightIndex).getResponse(), model, settings);
                    respExp.setWidth(width - textHorizontal);
                    toApply.add(new AsciiFrameElement(respExp, textHorizontal, currentVert + cmdExp.getHeight() + 1));
                    nextVert += respExp.getHeight() + 1;
                }
                if (highlightIndex < toHighlight.size() && toHighlight.get(highlightIndex).getCommand() == cmd) {
                    //toApply.add(new LineElement(highlightStyle, textHorizontal - 2, currentVert, textHorizontal - 2, nextVert - 2, strategy));
                    //toApply.add(new CharacterElement('\\', highlightStyle, textHorizontal - 2, nextVert - 2));
                    toApply.add(new CharacterElement('>', highlightStyle, textHorizontal - 2, currentVert));
                    toApply.add(new CharacterElement('<', highlightStyle, textHorizontal - 2, currentVert + cmdExp.getHeight() + 1));
                } else {
                    //toApply.add(new LineElement(defaultStyle, textHorizontal - 2, currentVert, textHorizontal - 2, nextVert - 2, strategy));
                    //toApply.add(new CharacterElement('\\', defaultStyle, textHorizontal - 2, nextVert - 2));
                    //toApply.add(new CharacterElement('/', defaultStyle, textHorizontal - 2, currentVert));
                    toApply.add(new CharacterElement('-', defaultStyle, textHorizontal - 2, currentVert));
                }
                toApply.add(new LineElement(defaultStyle, textHorizontal - 3, nextVert - 1, width - 1, nextVert - 1, strategy));
            }
            nextVert = Math.max(currentVert + spacing, nextVert);
            if (cmd.isEmpty()) {
                toApply.add(new CharacterElement('|', defaultStyle, element.getDepth().getDepth() * spacing, currentVert));
            } else {
                toApply.add(new CharacterElement('O', defaultStyle, element.getDepth().getDepth() * spacing, currentVert));
            }
            currentSuccessDepths.remove(element.getDepth());
            currentFailDepths.remove(element.getDepth());
            if (currentHighlightDepth == element.getDepth()) {
                currentHighlightDepth = null;
            }

            for (CommandGrapher.CommandDepth depth : currentSuccessDepths) {
                toApply.add(new CharacterElement('|', successStyle, depth.getDepth() * spacing, currentVert));
                toApply.add(new LineElement(successStyle, depth.getDepth() * spacing, currentVert,
                        depth.getDepth() * spacing, nextVert, strategy));
            }
            for (CommandGrapher.CommandDepth depth : currentFailDepths) {
                toApply.add(new CharacterElement('|', failStyle, depth.getDepth() * spacing, currentVert));
                toApply.add(new LineElement(failStyle, depth.getDepth() * spacing, currentVert,
                        depth.getDepth() * spacing, nextVert, strategy));
            }
            boolean drawSuccess = (!skipImpossiblePaths || cmd.canSucceed()) && cmd.getOnSuccess() != null;
            if (!skipImpossiblePaths && drawSuccess) {
                Command tmp = cmd.getOnSuccess();
                while (!elements.contains(commands.get(tmp)) && tmp.canSucceed() && tmp.getOnSuccess() != null && tmp.isEmpty()) {
                    tmp = tmp.getOnSuccess();
                }
                if (!elements.contains(commands.get(tmp))) {
                    drawSuccess = false;
                }
            }
            boolean drawFail = (!skipImpossiblePaths || cmd.canFail()) && cmd.getOnFail() != null;
            if (!skipImpossiblePaths && drawFail) {
                Command tmp = cmd.getOnFail();
                while (!elements.contains(commands.get(tmp)) && tmp.canSucceed() && tmp.getOnSuccess() != null && tmp.isEmpty()) {
                    tmp = tmp.getOnSuccess();
                }
                if (!elements.contains(commands.get(tmp))) {
                    drawSuccess = false;
                }
            }
            if (highlightIndex < toHighlight.size() - 1 && toHighlight.get(highlightIndex).getCommand() == cmd) {
                if (drawSuccess && toHighlight.get(highlightIndex + 1).getCommand() == cmd.getOnSuccess()) {
                    toApply.add(new LineElement(highlightStyle, element.getDepth().getDepth() * spacing, currentVert,
                            commands.get(cmd.getOnSuccess()).getDepth().getDepth() * spacing, nextVert, strategy));
                    currentHighlightDepth = commands.get(cmd.getOnSuccess()).getDepth();
                    drawSuccess = false;
                } else if (drawFail && toHighlight.get(highlightIndex + 1).getCommand() == cmd.getOnFail()) {
                    toApply.add(new LineElement(highlightStyle, element.getDepth().getDepth() * spacing, currentVert,
                            commands.get(cmd.getOnFail()).getDepth().getDepth() * spacing, nextVert, strategy));
                    currentHighlightDepth = commands.get(cmd.getOnFail()).getDepth();
                    drawFail = false;
                }
                highlightIndex++;
            }
            if (drawSuccess) {
                toApply.add(new LineElement(successStyle, element.getDepth().getDepth() * spacing, currentVert,
                        commands.get(cmd.getOnSuccess()).getDepth().getDepth() * spacing, nextVert, strategy));
                currentSuccessDepths.add(commands.get(cmd.getOnSuccess()).getDepth());
            }
            if (drawFail) {
                toApply.add(new LineElement(failStyle, element.getDepth().getDepth() * spacing, currentVert,
                        commands.get(cmd.getOnFail()).getDepth().getDepth() * spacing, nextVert, strategy));
                currentFailDepths.add(commands.get(cmd.getOnFail()).getDepth());
            }
            if (oldHLDepth != null && element.getDepth() != oldHLDepth) {
                toApply.add(new CharacterElement('|', highlightStyle, oldHLDepth.getDepth() * spacing, currentVert));
                toApply.add(new LineElement(highlightStyle, oldHLDepth.getDepth() * spacing, currentVert,
                        oldHLDepth.getDepth() * spacing, nextVert, strategy));
            }

            currentVert = nextVert;
        }
        this.resetToSize(width, currentVert);
        for (CanvasElement el : toApply) {
            el.apply();
        }
    }
}
