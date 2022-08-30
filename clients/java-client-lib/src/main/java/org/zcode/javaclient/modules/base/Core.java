package org.zcode.javaclient.modules.base;

import org.zcode.javaclient.builders.Command;
import org.zcode.javaclient.modules.base.Core.Activate.ActivateCommandBuilder;

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
