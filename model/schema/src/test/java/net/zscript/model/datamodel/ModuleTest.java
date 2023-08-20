package net.zscript.model.datamodel;

import static java.util.Objects.requireNonNull;
import static org.assertj.core.api.Assertions.assertThat;

import java.io.IOException;
import java.net.URL;

import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.CsvSource;

import com.fasterxml.jackson.databind.json.JsonMapper;

import net.zscript.model.datamodel.ZscriptDataModel.ModuleModel;
import net.zscript.model.loader.ModelLoader;

public class ModuleTest {
    private final JsonMapper jsonMapper = ModelLoader.createJsonMapper();

    @ParameterizedTest
    @CsvSource({
            "my-module.yaml,        Testing"
    })
    public void shouldLoad(String module, String expectedName) throws IOException {
        URL         resourceurl = requireNonNull(getClass().getResource("/zscript-datamodel/test-modulebank/" + module), "resourceStream");
        ModuleModel model          = jsonMapper.readValue(resourceurl, ModuleModel.class);

        assertThat(model.getName()).isEqualTo(expectedName);
    }
}
