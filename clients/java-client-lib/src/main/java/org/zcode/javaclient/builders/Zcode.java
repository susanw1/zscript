package org.zcode.javaclient.builders;

import java.util.HashMap;
import java.util.Map;

import org.zcode.javaclient.components.Command;
import org.zcode.javaclient.components.ResponseSequence;
import org.zcode.model.datamodel.ZcodeDataModel.CommandModel;
import org.zcode.model.datamodel.ZcodeDataModel.ModuleModel;
import org.zcode.model.loader.ModelLoader;
import org.zcode.model.loader.ModuleBank;

public class Zcode {

    private Map<String, ModuleBank> moduleBanks;

    private Zcode(ModelLoader modelLoader) {
        moduleBanks = modelLoader.getModuleBanks();
    }

    public static Zcode forModel(ModelLoader modelLoader) {
        return new Zcode(modelLoader);
    }

    public ModuleCommandFinder forModule(String moduleBankName, String moduleName) {
        return new ModuleCommandFinder(moduleBankName, moduleName);
    }

    public ResponseSequence parseResponse(Command c, String responseString) {
        return null;
//        return new ResponseSequence(responseString);
    }

    class ModuleCommandFinder {
        private final ModuleModel moduleModel;

        public ModuleCommandFinder(String moduleBankName, String moduleName) {
            this.moduleModel = moduleBanks.get(moduleBankName)
                    .getModule(moduleName);
        }

        public CommandBuilder makeCommand(String commandName) {
            return new CommandBuilder(moduleModel.getCommands()
                    .stream().filter(c -> c.getName().equals(commandName)).findFirst().orElseThrow(() -> new RuntimeException("unknown command: " + commandName)));
        }
    }

    static class CommandBuilder {
        private final CommandModel        model;
        private final Map<String, Object> keyValues = new HashMap<>();

        public CommandBuilder(CommandModel model) {
            this.model = model;
        }

        public CommandBuilder setField(String name, int value) {
            keyValues.put(name, value);
            return this;
        }

        public Command build() {
            return null;
        }
    }
}
