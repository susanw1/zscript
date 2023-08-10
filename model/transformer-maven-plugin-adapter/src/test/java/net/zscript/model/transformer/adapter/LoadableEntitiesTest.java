package net.zscript.model.transformer.adapter;

import static java.util.Collections.singletonList;
import static org.assertj.core.api.Assertions.assertThat;
import static org.assertj.core.api.Assertions.assertThatExceptionOfType;
import static org.assertj.core.api.Assertions.tuple;

import java.nio.file.FileSystem;
import java.nio.file.Path;
import java.util.List;

import org.junit.jupiter.api.Test;

import com.google.common.jimfs.Configuration;
import com.google.common.jimfs.Jimfs;

import net.zscript.model.transformer.adapter.LoadableEntities.LoadedEntityContent;

class LoadableEntitiesTest {
    private final FileSystem fs = Jimfs.newFileSystem(Configuration.unix());

    @Test
    void shouldProductListOfLoadedEntities() {
        FileSystem fs = Jimfs.newFileSystem(Configuration.unix());

        Path       rootPath = fs.getPath("/foo");
        List<Path> relPaths = List.of(fs.getPath("bar"), fs.getPath("baz/"));
        String     suffix   = "java";

        LoadableEntities le = new LoadableEntities("desc", rootPath, relPaths, suffix);

        final List<LoadedEntityContent<String>> result = le.loadEntities(entity -> {
            return singletonList(entity.withContent(
                    "content+" + entity.getRelativePath(),
                    fs.getPath("a").resolve(entity.getRelativePath())));
        });

        assertThat(result)
                .hasSize(2)
                .extracting(LoadedEntityContent<String>::getContent,
                        LoadedEntityContent<String>::getRelativeOutputPath,
                        LoadedEntityContent<String>::getDescription,
                        LoadedEntityContent<String>::getRelativePath,
                        LoadedEntityContent<String>::getRootPath,
                        LoadedEntityContent<String>::getFullPath)
                .containsExactly(
                        tuple("content+bar", fs.getPath("a/bar"), "desc", fs.getPath("bar"), fs.getPath("/foo"), fs.getPath("/foo/bar")),
                        tuple("content+baz", fs.getPath("a/baz"), "desc", fs.getPath("baz"), fs.getPath("/foo"), fs.getPath("/foo/baz")));
    }

    @Test
    void shouldRejectAbsoluteEntityPath() {
        assertThatExceptionOfType(IllegalArgumentException.class).isThrownBy(() -> {
            new LoadableEntities("desc", fs.getPath("/foo"), List.of(fs.getPath("/bar")), "java").loadEntities(e -> List.of());
        });
    }

    @Test
    void shouldRejectAbsoluteOutputPath() {
        assertThatExceptionOfType(IllegalArgumentException.class).isThrownBy(() -> {
            new LoadableEntities("desc", fs.getPath("/foo"), List.of(fs.getPath("bar")), "java")
                    .loadEntities(e -> List.of(e.withContent("", fs.getPath("/baz"))));
        });
    }
}
