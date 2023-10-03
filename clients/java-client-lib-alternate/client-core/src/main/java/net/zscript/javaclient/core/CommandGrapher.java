package net.zscript.javaclient.core;

import java.util.List;
import java.util.Map;

import net.zscript.ascii.AsciiFrame;
import net.zscript.model.ZscriptModel;

public interface CommandGrapher {
    AsciiFrame graph(ZscriptModel model, Map<Command, CommandGraphElement> commands, List<CommandGraphElement> elements,
            CommandDepth maxDepth, List<Command> toHighlight, List<Response> responses, CommandGraph.GraphPrintSettings settings);

    public class CommandDepth {
        private int depth;

        CommandDepth(CommandDepth source) {
            this.depth = source.depth + 1;
        }

        CommandDepth(int depth) {
            this.depth = depth;
        }

        public void depthGreaterThan(CommandDepth other) {
            if (other.depth + 1 > depth) {
                depth = other.depth + 1;
            }
        }

        public int getDepth() {
            return depth;
        }

        public void setDepth(int depth) {
            this.depth = depth;
        }
    }

    public class CommandGraphElement {
        private final Command command;

        private final CommandDepth depth;

        CommandGraphElement(Command command, CommandDepth depth) {
            this.command = command;
            this.depth = depth;
        }

        public Command getCommand() {
            return command;
        }

        public CommandDepth getDepth() {
            return depth;
        }

    }
}
