package org.zcode.model.datamodel;

import java.util.List;

public interface DefinitionResources {
    List<ModuleBank> getModuleBanks();

    interface ModuleBank {
        String getName();

        List<String> getModules();
    }
}
