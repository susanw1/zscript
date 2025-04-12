package net.zscript.model.datamodel;

import java.io.IOException;
import java.io.InputStream;
import java.util.List;
import java.util.Map;

import static java.util.Objects.requireNonNull;
import static org.assertj.core.api.Assertions.assertThat;

import com.fasterxml.jackson.databind.json.JsonMapper;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

import net.zscript.model.datamodel.IntrinsicsDataModel.Intrinsics;
import net.zscript.model.datamodel.IntrinsicsDataModel.ZcharDefs;
import net.zscript.model.loader.ModelLoader;

public class IntrinsicsTest {
    private final JsonMapper          jsonMapper = ModelLoader.createJsonMapper();
    private       IntrinsicsDataModel dataModel;

    @BeforeEach
    public void setup() throws IOException {
        final InputStream resourceStream = requireNonNull(getClass().getResourceAsStream("/zscript-datamodel/intrinsics.yaml"), "resourceStream is null");
        dataModel = jsonMapper.readValue(resourceStream, IntrinsicsDataModel.class);
    }

    @Test
    public void shouldLoadIntrinsics() {
        final Intrinsics intrinsics = dataModel.getIntrinsics();
        assertThat(intrinsics.getRequestFields()).hasSize(1);
        assertThat(intrinsics.getResponseFields()).hasSize(1);
        assertThat(intrinsics.getZchars()).hasSize(19);
        assertThat(intrinsics.getStatus()).hasSize(33);
    }

    @Test
    public void shouldEscapeChars() {
        final Intrinsics intrinsics = dataModel.getIntrinsics();
        assertThat(intrinsics.getZchars()).isNotEmpty();
        final List<ZcharDefs> zchs = intrinsics.getZchars();

        Map<Character, Character> escapes = Map.of('\n', 'n', '\0', '0', '\t', 't', '\r', 'r');
        for (ZcharDefs zcd : zchs) {
            if (escapes.containsKey(zcd.getCh())) {
                assertThat(zcd.getChEscapedAsC()).isEqualTo("\\" + escapes.get(zcd.getCh()));
            } else {
                assertThat(zcd.getChEscapedAsC()).isEqualTo("" + zcd.getCh());
            }
        }
    }
}
