package net.zscript.model.datamodel;

import java.util.List;

public interface DefinitionResources {
    List<ModuleBankDef> getModuleBanks();

    interface ModuleBankDef {
        String getName();

        int getId();

        List<String> getModuleDefinitions();
    }
}
