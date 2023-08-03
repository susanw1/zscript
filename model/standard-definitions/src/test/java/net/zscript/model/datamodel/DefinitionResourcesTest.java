package net.zscript.model.datamodel;

import static java.util.Objects.requireNonNull;
import static org.assertj.core.api.Assertions.assertThat;

import java.io.IOException;
import java.io.InputStream;
import java.util.List;

import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

import com.fasterxml.jackson.databind.MapperFeature;
import com.fasterxml.jackson.databind.json.JsonMapper;
import com.fasterxml.jackson.dataformat.yaml.YAMLFactory;
import com.fasterxml.jackson.module.mrbean.MrBeanModule;

import net.zscript.model.datamodel.DefinitionResources.ModuleBankDef;

public class DefinitionResourcesTest {
    private JsonMapper jsonMapper;

    @BeforeEach
    public void setup() {
        jsonMapper = JsonMapper.builder(new YAMLFactory())
                .enable(MapperFeature.ACCEPT_CASE_INSENSITIVE_ENUMS)
                .addModule(new MrBeanModule())
                .build();
    }

    @Test
    public void shouldLoadDefinitionResources() throws IOException {
        final InputStream   resourceStream = requireNonNull(getClass().getResourceAsStream("/datamodel/module-list.yaml"), "resourceStream");
        DefinitionResources model          = jsonMapper.readValue(resourceStream, DefinitionResources.class);

        final List<ModuleBankDef> moduleBanks = model.getModuleBanks();
        assertThat(moduleBanks).hasSize(1);
        assertThat(moduleBanks.get(0).getModuleDefinitions()).hasSize(3);
    }
}
