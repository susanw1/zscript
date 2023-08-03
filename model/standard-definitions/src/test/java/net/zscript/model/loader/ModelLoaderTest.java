package net.zscript.model.loader;

import static org.assertj.core.api.Assertions.assertThat;

import java.io.IOException;

import org.junit.jupiter.api.Test;

import net.zscript.model.loader.ModelLoader;

class ModelLoaderTest {

    @Test
    void should() throws IOException {
        ModelLoader ml = ModelLoader.standardModel();
        assertThat(ml.getModuleBanks()).hasSize(1);
        assertThat(ml.getModuleBanks().get("Base").getModule("Core").getCommands()).hasSize(9);
        assertThat(ml.getModuleBanks().get("Base").getModule("Core").getModuleBank()).isNotNull();
    }
}
