package net.zscript.model.datamodel;

import static java.util.Objects.requireNonNull;
import static org.assertj.core.api.Assertions.assertThat;

import java.io.IOException;
import java.io.InputStream;

import org.junit.jupiter.api.Test;

import com.fasterxml.jackson.databind.json.JsonMapper;

import net.zscript.model.loader.ModelLoader;

public class IntrinsicsTest {
    private final JsonMapper jsonMapper = ModelLoader.createJsonMapper();

    @Test
    public void shouldLoadIntrinsics() throws IOException {
        final InputStream   resourceStream = requireNonNull(getClass().getResourceAsStream("/zscript-datamodel/intrinsics.yaml"), "resourceStream is null");
        IntrinsicsDataModel dataModel      = jsonMapper.readValue(resourceStream, IntrinsicsDataModel.class);

        assertThat(dataModel.getIntrinsics().getRequestFields()).hasSize(1);
        assertThat(dataModel.getIntrinsics().getResponseFields()).hasSize(1);
        assertThat(dataModel.getIntrinsics().getZchars()).hasSize(21);
        assertThat(dataModel.getIntrinsics().getStatus()).hasSize(33);
    }
}
