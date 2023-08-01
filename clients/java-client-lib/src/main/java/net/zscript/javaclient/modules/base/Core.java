package net.zscript.javaclient.modules.base;

import net.zscript.javaclient.builders.Command;
import net.zscript.javaclient.modules.base.Core.Activate.ActivateCommandBuilder;

// WIP
public interface Core {

    ActivateCommandBuilder activate();

    public class Activate extends ExtendableCommand {
        public interface ActivateCommandBuilder extends Builder<Activate> {
            ActivateCommandBuilder key(int key);
        }

        public interface ActivateResponse {
            boolean previousActivationState();
        }
    }
}

interface Builder<T extends ExtendableCommand> {
    T build();
}

class ExtendableCommand extends Command {
}

interface CommandsSet {

}
