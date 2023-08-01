package net.zscript.model.loader;

import java.util.HashMap;
import java.util.Map;

import net.zscript.model.datamodel.DefinitionResources.ModuleBankDef;
import net.zscript.model.datamodel.ZcodeDataModel.ModuleModel;

public class ModuleBank {
    final ModuleBankDef mbDef;

    final Map<String, ModuleModel> modules = new HashMap<>();

    public ModuleBank(ModuleBankDef mbDef) {
        this.mbDef = mbDef;
    }

    public void add(ModuleModel module) {
        modules.put(module.getName(), module);
    }

    public String getName() {
        return mbDef.getName();
    }

    public int getId() {
        return mbDef.getId();
    }

    public ModuleModel getModule(String name) {
        return modules.get(name);
    }
}