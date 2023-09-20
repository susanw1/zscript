package net.zscript.model.datamodel;

import net.zscript.model.ZscriptModel;
import net.zscript.model.loader.ModuleBank;

public class ModelValidator {
    public void check(ZscriptModel model) {
        for (ModuleBank bank : model.banks()) {
            if (!fitsBits(bank.getId(), 8)) {
                throw new ZscriptModelException("ModuleBank Id should be 0x00-0xff [bank=%s, id=%d]", bank.getName(), bank.getId());
            }
            if (!stringUpperCamel(bank.getName())) {
                throw new ZscriptModelException("Modules and ModuleBank names should be upper-camel-case, eg deviceReset [bank=%s, id=%d]", bank.getName(), bank.getId());
            }
        }
    }

    private boolean stringLowerCamel(String name) {
        return name != null && !name.isEmpty() && name.matches("^\\p{Lower}\\p{Alnum}*$");
    }

    private boolean stringUpperCamel(String name) {
        return name != null && !name.isEmpty() && name.matches("^\\p{Upper}\\p{Alnum}*$");
    }

    boolean fitsBits(int value, int nBits) {
        return (value & ~(1 << nBits)) == 0;
    }
}
