package org.zcode.javaclient.modules;

public interface Core {
    class Configuration extends ExtendableCommand {
        interface ConfigurationCommand {
        }

        interface ConfigurationResponse {
            CommandsSet commandsSet();
        }
    }

    class Echo extends ExtendableCommand {
        interface EchoCommand {

        }

        interface EchoResponse {

        }
    }

    class Activate extends ExtendableCommand {
        interface ActivateCommand {
            ActivateCommand key(int key);
        }

        interface ActivateResponse {
            boolean previousActivationState();
        }
    }
}

class ExtendableCommand {

}

interface CommandsSet {

}
