package net.zscript.javaclient.commandprinting;

import net.zscript.javaclient.commandpaths.CommandElement;
import net.zscript.javaclient.commandpaths.CommandExecutionPath;
import net.zscript.javaclient.commandpaths.ResponseElement;
import net.zscript.javaclient.commandpaths.ResponseExecutionPath;
import net.zscript.model.ZscriptModel;

/**
 * @param <T> The output type from the graphing and explaining system
 * @param <C> The settings type for the command explaining system
 * @param <S> The settings type for the graph drawing system
 */
public interface CommandGrapher<T, C, S> {
    T graph(CommandExecutionPath path, S settings);

    T graph(CommandExecutionPath path, ResponseExecutionPath resps, S settings);

    T explainCommand(CommandElement target, ZscriptModel model, C settings);

    T explainResponse(CommandElement source, ResponseElement target, ZscriptModel model, C settings);

    S getDefaultGraphSettings();

    S getDefaultGraphSettings(boolean skipImpossiblePaths);

    S getDefaultGraphSettings(C explainerSettings, boolean skipImpossiblePaths);

    C getDefaultExplainerSettings();

    C getDefaultExplainerSettings(VerbositySetting v);

    class CommandDepth {
        private int depth;

        CommandDepth(CommandDepth source) {
            this.depth = source.depth + 1;
        }

        public CommandDepth(int depth) {
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

    class CommandGraphElement {
        private final CommandElement command;

        private final CommandDepth depth;

        CommandGraphElement(CommandElement command, CommandDepth depth) {
            this.command = command;
            this.depth = depth;
        }

        public CommandElement getCommand() {
            return command;
        }

        public CommandDepth getDepth() {
            return depth;
        }

    }
}
