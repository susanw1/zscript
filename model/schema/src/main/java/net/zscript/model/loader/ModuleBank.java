package net.zscript.model.loader;

import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Optional;

import static java.util.Comparator.comparing;
import static java.util.stream.Collectors.toList;

import com.fasterxml.jackson.annotation.JsonProperty;

import net.zscript.model.datamodel.DefinitionResources.ModuleBankDef;
import net.zscript.model.datamodel.ZscriptDataModel.ModuleModel;
import net.zscript.model.datamodel.ZscriptModelException;

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
        ModuleModel oldByName = modulesByName.put(module.getName(), module);
        if (oldByName != null && oldByName.getId() != module.getId()) {
            throw new ZscriptModelException("Module already exists with different ID [module=%s, old id=%d, new id=%d]",
                    module.getName(), oldByName.getId(), module.getId());
        }
        ModuleModel oldById = modulesById.put((int) module.getId(), module);
        if (oldById != null && !oldById.getName().equals(module.getName())) {
            throw new ZscriptModelException("Module ID already exists with different name [module id=%d, old name=%s, new name=%s]",
                    module.getId(), oldById.getName(), module.getName());
        }
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

    @JsonProperty
    public List<ModuleModel> modules() {
        return modulesByName.values().stream().sorted(comparing(ModuleModel::getId)).collect(toList());
    }
}
