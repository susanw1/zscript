package net.zscript.javaclient.core;

import java.util.List;
import java.util.Map;

public interface CommandGrapher {
    String graph(Map<CommandExecutionPath.Command, CommandGraphElement> commands, List<CommandGraphElement> elements,
            CommandDepth maxDepth, List<CommandExecutionPath.Command> toHighlight);

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
    }

    public class CommandGraphElement {
        private final CommandExecutionPath.Command command;

        private final CommandDepth depth;

        CommandGraphElement(CommandExecutionPath.Command command, CommandDepth depth) {
            this.command = command;
            this.depth = depth;
        }

        public CommandExecutionPath.Command getCommand() {
            return command;
        }

        public CommandDepth getDepth() {
            return depth;
        }

    }
}
