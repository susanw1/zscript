package net.zscript.model.datamodel;

import static java.util.Objects.requireNonNull;
import static org.assertj.core.api.Assertions.assertThat;

import java.io.IOException;
import java.io.InputStream;

import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.CsvSource;

import com.fasterxml.jackson.databind.MapperFeature;
import com.fasterxml.jackson.databind.json.JsonMapper;
import com.fasterxml.jackson.dataformat.yaml.YAMLFactory;
import com.fasterxml.jackson.module.mrbean.MrBeanModule;

public class ModuleTest {
    private JsonMapper jsonMapper;

    @BeforeEach
    public void setup() {
        jsonMapper = JsonMapper.builder(new YAMLFactory())
                .enable(MapperFeature.ACCEPT_CASE_INSENSITIVE_ENUMS)
                .addModule(new MrBeanModule())
                .build();
    }

    @ParameterizedTest
    @CsvSource({
            "000x-core.yaml,        Core",
            "001x-outercore.yaml,   OuterCore",
            "005x-i2c.yaml,         I2C"
    })
    public void shouldLoad_000x(String module, String expectedName) throws IOException {
        InputStream      resourceStream = requireNonNull(getClass().getResourceAsStream("/datamodel/00xx-base-modules/" + module), "resourceStream");
        ZscriptDataModel model          = jsonMapper.readValue(resourceStream, ZscriptDataModel.class);

        assertThat(model.getModules().get(0).getName()).isEqualTo(expectedName);
    }
}