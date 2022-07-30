package org.zcode.javaclient.builders;

import java.util.HashMap;
import java.util.Map;

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

    public Response makeResponse(Command c, String string) {
        return new Response();
    }

    class ModuleCommandFinder {
        private final ModuleModel module;

        public ModuleCommandFinder(String moduleBankName, String moduleName) {
            this.module = moduleBanks.get(moduleBankName)
                    .getModule(moduleName);
        }

        public CommandBuilder makeCommand(String commandName) {
            return new CommandBuilder(module.getCommands()
                    .stream().filter(c -> c.getName().equals(commandName)).findFirst().orElseThrow(() -> new RuntimeException()));
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
            return new Command();
        }

    }

}
