package net.zscript.javaclient.core;

import java.util.Arrays;
import java.util.List;
import java.util.Map;

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

    public String graph(Map<CommandExecutionPath.Command, CommandGraphElement> commands, List<CommandGraphElement> elements,
            CommandDepth maxDepth, List<CommandExecutionPath.Command> toHighlight, boolean skipImpossiblePaths) {
        StringBuilder output        = new StringBuilder();
        boolean[]     hasLane       = new boolean[maxDepth.getDepth() + 1];
        int           highlightLane = -1;
        hasLane[0] = true;
        int highlightIndex = 0;
        for (CommandGrapher.CommandGraphElement element : elements) {
            // Set up variables
            int elementDepth = element.getDepth().getDepth();

            CommandExecutionPath.Command successCmd = element.getCommand().getEndLink().getOnSuccess();
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
            CommandExecutionPath.Command failCmd = element.getCommand().getEndLink().getOnFail();

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
