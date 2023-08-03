package net.zscript.model.datamodel;

import static java.util.Objects.requireNonNull;
import static org.assertj.core.api.Assertions.assertThat;

import java.io.IOException;
import java.io.InputStream;

import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

import com.fasterxml.jackson.databind.MapperFeature;
import com.fasterxml.jackson.databind.json.JsonMapper;
import com.fasterxml.jackson.dataformat.yaml.YAMLFactory;
import com.fasterxml.jackson.module.mrbean.MrBeanModule;

import net.zscript.model.datamodel.IntrinsicsDataModel;

public class IntrinsicsTest {
    private JsonMapper jsonMapper;

    @BeforeEach
    public void setup() {
        jsonMapper = JsonMapper.builder(new YAMLFactory())
                .enable(MapperFeature.ACCEPT_CASE_INSENSITIVE_ENUMS)
                .addModule(new MrBeanModule())
                .build();
    }

    @Test
    public void shouldLoadIntrinsics() throws IOException {

        final InputStream   resourceStream = requireNonNull(getClass().getResourceAsStream("/intrinsics.yaml"), "resourceStream");
        IntrinsicsDataModel model          = jsonMapper.readValue(resourceStream, IntrinsicsDataModel.class);

        assertThat(model.getIntrinsics().getRequestParams()).hasSize(2);
        assertThat(model.getIntrinsics().getResponseParams()).hasSize(2);
        assertThat(model.getIntrinsics().getStatus()).hasSize(32);
    }
}