package net.zscript.model;

import java.util.Optional;

import static org.assertj.core.api.Assertions.assertThat;

import org.junit.jupiter.api.Test;

import net.zscript.model.datamodel.ZscriptDataModel.ModuleModel;

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
        Optional<ModuleModel> module = model.getModuleBank("Base").orElseThrow().getModule("Testing");
        assertThat(module).isPresent();

        assertThat(module.get().getNotifications().get(2).getSections()).hasSize(2);
    }

}
