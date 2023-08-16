package net.zscript.model;

import static org.assertj.core.api.Assertions.assertThat;

import org.junit.jupiter.api.Test;

class ZscriptModelTest {
    @Test
    void shouldLoadRaw() {
        ZscriptModel model = ZscriptModel.rawModel();
        assertThat(model.getIntrinsics().getStatus()).hasSize(32);
        assertThat(model.getIntrinsics().getRequestFields()).hasSize(1);
        assertThat(model.getIntrinsics().getResponseFields()).hasSize(1);
    }

    @Test
    void shouldLoadDefinition() {
        ZscriptModel model = ZscriptModel.standardModel();
        assertThat(model.getModuleBank("Base")).isPresent();
        assertThat(model.getModuleBank("Banana")).isNotPresent();
        assertThat(model.getModuleBank("Base").get().getModule("Testing")).isPresent();
    }

}
