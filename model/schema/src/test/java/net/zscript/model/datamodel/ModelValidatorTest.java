package net.zscript.model.datamodel;

import static org.junit.jupiter.api.Assertions.*;

import org.junit.jupiter.api.Test;

import net.zscript.model.ZscriptModel;

class ModelValidatorTest {

    @Test
    void shouldCheck() {
        ZscriptModel model = ZscriptModel.standardModel();
        new ModelValidator().check(model);
    }
}
