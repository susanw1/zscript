package net.zscript.model.transformer.adapter;

import static org.assertj.core.api.Assertions.assertThat;

import java.io.BufferedWriter;
import java.io.IOException;
import java.nio.file.FileSystem;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.List;
import java.util.Map;

import org.junit.jupiter.api.Test;

import com.google.common.jimfs.Configuration;
import com.google.common.jimfs.Jimfs;

import net.zscript.model.transformer.adapter.LoadableEntities.LoadedEntityContent;

class YamlTransformerPluginMapperTest {
    private final FileSystem fs = Jimfs.newFileSystem(Configuration.unix());

    @Test
    public void shouldProductListOfLoadedEntities() throws IOException {
        FileSystem fs = Jimfs.newFileSystem(Configuration.unix());

        Path       rootPath = fs.getPath("/foo");
        List<Path> relPaths = List.of(fs.getPath("baz/a.yaml"));
        String     suffix   = "java";

        LoadableEntities            le     = new LoadableEntities("desc", rootPath, relPaths, suffix);
        YamlTransformerPluginMapper mapper = new YamlTransformerPluginMapper();

        final Path yamlFile = fs.getPath("/foo", "baz", "a.yaml");
        Files.createDirectories(yamlFile.getParent());
        try (final BufferedWriter w = Files.newBufferedWriter(yamlFile)) {
            w.write("{a : w1, b: [x,y], c: w3}");
        }

        List<LoadedEntityContent<Map<?, ?>>> loadedEntities = mapper.loadAndMap(le);

        assertThat(loadedEntities).hasSize(1);
        final Map<?, ?> content = loadedEntities.get(0).getContent();
        assertThat(content.get("a")).isEqualTo("w1");
        assertThat(content.get("b")).isInstanceOf(List.class).isEqualTo(List.of("x", "y"));
        assertThat(content.get("c")).isEqualTo("w3");

        assertThat(loadedEntities.get(0).getRelativeOutputPath()).isEqualTo(fs.getPath("baz/a.java"));
    }
}
