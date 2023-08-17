package net.zscript.model.datamodel;

import static java.util.Objects.requireNonNull;
import static org.assertj.core.api.Assertions.assertThat;

import java.io.IOException;
import java.io.InputStream;
import java.util.List;

import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

import com.fasterxml.jackson.databind.json.JsonMapper;

import net.zscript.model.ZscriptModel;
import net.zscript.model.datamodel.DefinitionResources.ModuleBankDef;
import net.zscript.model.datamodel.ZscriptDataModel.CommandModel;
import net.zscript.model.datamodel.ZscriptDataModel.ModuleModel;
import net.zscript.model.datamodel.ZscriptDataModel.RequestFieldModel;
import net.zscript.model.loader.ModelLoader;

public class DefinitionResourcesTest {
    private final JsonMapper jsonMapper = ModelLoader.createJsonMapper();

    private InputStream         resourceStream;
    private DefinitionResources model;

    @BeforeEach
    public void setup() throws IOException {
        resourceStream = requireNonNull(getClass().getResourceAsStream("/zscript-datamodel/module-list.yaml"), "resourceStream");
        model = jsonMapper.readValue(resourceStream, DefinitionResources.class);
    }

    @Test
    public void shouldLoadDefinitionResources() throws IOException {
        final List<ModuleBankDef> moduleBanks = model.getModuleBanks();
        assertThat(moduleBanks).hasSize(1);
        assertThat(moduleBanks.get(0).getModuleDefinitions()).hasSize(1);
    }

    @Test
    public void shouldMatchCommandType() throws IOException {
        final ZscriptModel standardModel = ZscriptModel.standardModel();
        ModuleModel        module        = standardModel.getModule("Base", "Testing").orElseThrow();

        CommandModel      command = module.getCommands().stream().filter(c -> c.getCommandName().equals("capabilities")).findFirst().orElseThrow();
        RequestFieldModel req     = command.getRequestFields().stream().filter(f -> f.getName().equals("versionType")).findFirst().orElseThrow();
        assertThat(req.getKey()).isEqualTo('V');
    }
}
