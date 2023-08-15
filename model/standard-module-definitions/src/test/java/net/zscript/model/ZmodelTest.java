package net.zscript.model;

import static net.zscript.model.standard.StandardModel.MODULEBANK_BASE_NAME;
import static net.zscript.model.standard.StandardModel.MODULE_BASE_CORE_NAME;
import static org.assertj.core.api.Assertions.assertThat;

import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

import net.zscript.model.loader.ModuleBank;

class ZmodelTest {

    @BeforeEach
    void setUp() throws Exception {
    }

    @Test
    void shouldLoadRaw() {
        ZscriptModel model = ZscriptModel.rawModel();
        assertThat(model.getIntrinsics().getStatus()).hasSize(32);
        assertThat(model.getIntrinsics().getRequestFields()).hasSize(1);
        assertThat(model.getIntrinsics().getResponseFields()).hasSize(1);
    }

    @Test
    void shouldLoadDefinitionAndFetchModules() {
        ZscriptModel model = ZscriptModel.standardModel();
        assertThat(model.getModuleBank(MODULEBANK_BASE_NAME)).isPresent();
        assertThat(model.getModuleBank(MODULEBANK_BASE_NAME).get().getModule(MODULE_BASE_CORE_NAME)).isPresent();
        assertThat(model.getModule(MODULEBANK_BASE_NAME, MODULE_BASE_CORE_NAME)).isPresent();
        assertThat(model.getModule(MODULEBANK_BASE_NAME, "Boo")).isNotPresent();
        assertThat(model.getModule("Foo", MODULE_BASE_CORE_NAME)).isNotPresent();
    }

    @Test
    void shouldLoadAllBaseModules() {
        ZscriptModel model = ZscriptModel.standardModel();
        ModuleBank   mb    = model.getModuleBank(MODULEBANK_BASE_NAME).orElseThrow();
        assertThat(mb.modules()).extracting(m -> m.getModuleName())
                .containsExactlyInAnyOrder("Core", "OuterCore", "ScriptSpace", "Pins", "I2C", "Serial", "Servo");
    }
}
