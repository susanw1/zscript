package net.zscript.javaclient.builders;

import java.util.HashMap;
import java.util.Map;

import net.zscript.model.datamodel.ZcodeDataModel.CommandModel;
import net.zscript.model.datamodel.ZcodeDataModel.ModuleModel;
import net.zscript.model.loader.ModelLoader;
import net.zscript.model.loader.ModuleBank;

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

    public Response parseResponse(Command c, String responseString) {
        return new Response(responseString);
    }

//    public <C extends ZcodeModule> C forModule(Class<C> moduleType) {
//        ModuleCommandFinderHandler handler = new ModuleCommandFinderHandler();
//        return moduleType.cast(Proxy.newProxyInstance(getClass().getClassLoader(), new Class<?>[] { moduleType }, handler));
//    }
//
//    class ModuleCommandFinderHandler implements InvocationHandler {
//        @Override
//        public Object invoke(Object moduleProxy, Method method, Object[] args) throws Throwable {
//            System.out.println("proxy method called: " + method);
//
//            final String commandName = method.getName();
//
//            final String packageName    = moduleProxy.getClass().getPackageName();
//            final String trailingPart   = packageName.substring(packageName.lastIndexOf('.') + 1);
//            final String moduleBankName = Character.toUpperCase(trailingPart.charAt(0)) + trailingPart.substring(1);
//
//            final ModuleBank  moduleBank  = moduleBanks.get(moduleBankName);
//            final ModuleModel moduleModel = moduleBank.getModule(moduleProxy.getClass().getSimpleName());
//
//            final CommandModel commandModel = moduleModel.getCommands().stream()
//                    .filter(cmd -> cmd.getName().equals(commandName))
//                    .findFirst()
//                    .orElseThrow(() -> new RuntimeException("unknown command: " + commandName));
//
//            final String commandClassName = Character.toUpperCase(commandName.charAt(0)) + commandName.substring(1);
//
//            final Class<?> commandClass = stream(moduleProxy.getClass().getDeclaredClasses())
//                    .filter(c -> c.getSimpleName().equals(commandClassName))
//                    .findFirst()
//                    .orElseThrow(() -> new RuntimeException("unknown command: " + commandName));
//            return null;
//        }
//    }

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
            return new Command();
        }
    }
}
