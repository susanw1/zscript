package net.zscript.model.loader;

import static java.util.Comparator.comparing;
import static java.util.stream.Collectors.toList;

import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Optional;

import net.zscript.model.datamodel.DefinitionResources.ModuleBankDef;
import net.zscript.model.datamodel.ZscriptDataModel.ModuleModel;

/**
 * Stores the models for a collection of up to 16 Modules.
 */
public class ModuleBank {
    final ModuleBankDef mbDef;

    final Map<String, ModuleModel>  modulesByName = new HashMap<>();
    final Map<Integer, ModuleModel> modulesById   = new HashMap<>();

    public ModuleBank(ModuleBankDef mbDef) {
        this.mbDef = mbDef;
    }

    void add(ModuleModel module) {
        modulesByName.put(module.getName(), module);
        modulesById.put((int) module.getId(), module);
    }

    public String getName() {
        return mbDef.getName();
    }

    public List<String> getDefaultPackage() {
        return mbDef.getDefaultPackage();
    }

    public int getId() {
        return mbDef.getId();
    }

    public Optional<ModuleModel> getModule(String name) {
        return Optional.ofNullable(modulesByName.get(name));
    }

    public Optional<ModuleModel> getModule(int moduleId) {
        return Optional.ofNullable(modulesById.get(moduleId));
    }

    public List<ModuleModel> modules() {
        return modulesByName.values().stream().sorted(comparing(ModuleModel::getId)).collect(toList());
    }
}
