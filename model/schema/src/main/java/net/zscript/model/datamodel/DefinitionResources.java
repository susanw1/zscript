package net.zscript.model.datamodel;

import java.util.List;

/**
 * Defines the resources that should be loaded
 */
public interface DefinitionResources {
    List<ModuleBankDef> getModuleBanks();

    interface ModuleBankDef {
        /** The name of this Module Bank. */
        String getName();

        /** The module bank ID of this Module Bank (defines upper byte of the (2-byte) command id). Value 0x00-0xff */
        int getId();

        /** List of words making up the package-name for this module bank. */
        List<String> getDefaultPackage();

        /** Relative paths to JSON/YAML files defining the Modules in this bank. */
        List<String> getModuleDefinitions();
    }
}
