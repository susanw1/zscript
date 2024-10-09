package net.zscript.model.templating.contextloader;

import java.io.BufferedWriter;
import java.io.IOException;
import java.io.UncheckedIOException;
import java.net.URI;
import java.net.URISyntaxException;
import java.nio.file.FileSystem;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.List;

import static java.util.Collections.singletonList;
import static org.assertj.core.api.Assertions.assertThat;
import static org.assertj.core.api.Assertions.assertThatThrownBy;

import com.google.common.jimfs.Configuration;
import com.google.common.jimfs.Jimfs;
import org.junit.jupiter.api.Test;

import net.zscript.maven.templating.contextloader.LoadableEntities;
import net.zscript.maven.templating.contextloader.LoadableEntities.LoadedEntityScopes;
import net.zscript.model.datamodel.IntrinsicsDataModel.Intrinsics;
import net.zscript.model.datamodel.ZscriptDataModel.ModuleModel;

class ZscriptModuleTemplatingContextLoaderTest {
    ZscriptModuleTemplatingContextLoader contextLoader = new ZscriptModuleTemplatingContextLoader();

    private final FileSystem fs = Jimfs.newFileSystem(Configuration.unix());

    @Test
    public void shouldProduceListOfLoadedContextFromModules() throws IOException {
        final Path rootDirPath = Files.createDirectory(fs.getPath("/foo"));

        final Path moduleList = rootDirPath.resolve("module-list.yaml");

        final Path moduleDef = moduleList.resolveSibling("sub/testmod.yaml");
        Files.createDirectories(moduleDef.getParent());

        // Create a little moduleList in memory!
        try (final BufferedWriter w = Files.newBufferedWriter(moduleList)) {
            w.write("moduleBanks: [ {name: Mod, id: 0x05, defaultPackage: [com,example], moduleDefinitions: [ sub/testmod.yaml ] } ]");
        }

        // And create a tiny module, in memory!
        try (final BufferedWriter w = Files.newBufferedWriter(moduleDef)) {
            w.write("{ name: Testing, id: 0xb, version: 35, description: mm, commands: [ { name: c1, command: 6, description: cc, "
                    + "requestFields: [ {key: V, name: f1, description: ff, typeDefinition: { \"@type\": number } } ], responseFields: []  } ], notifications: [] }");
        }

        final LoadableEntities le = new LoadableEntities(rootDirPath.toUri(), List.of(moduleList.getFileName().toString()), "txt", fs);

        // load it all, including the classpath Intrinsics
        final List<LoadedEntityScopes> loadedEntities = contextLoader.loadAndMap(le);

        // spot checks to make sure it's all there
        assertThat(loadedEntities).hasSize(1);
        final LoadedEntityScopes loadedEntityScopes = loadedEntities.get(0);
        assertThat(loadedEntityScopes.getRelativeOutputPath().toString()).isEqualTo("com/example/mod/TestingModule.txt");
        assertThat(loadedEntityScopes.getScopes()).hasSize(3);
        assertThat(loadedEntityScopes.getScopes().get(0)).isInstanceOf(ConversionHelper.class);
        assertThat(loadedEntityScopes.getScopes().get(1)).isInstanceOf(Intrinsics.class);
        assertThat(loadedEntityScopes.getScopes().get(2)).isInstanceOf(ModuleModel.class);

        assertThat(((ConversionHelper) loadedEntityScopes.getScopes().get(0)).getAdditional()).containsEntry("package-elements", "com.example.mod");
        assertThat(((Intrinsics) loadedEntityScopes.getScopes().get(1)).getZchars()).isNotNull().isNotEmpty();
        assertThat(((ModuleModel) loadedEntityScopes.getScopes().get(2)).getCommandById(6).orElseThrow().getFullCommand()).isEqualTo(0x5b6);
    }

    @Test
    public void shouldProduceListOfLoadedContextFromIntrinsics() throws IOException {
        final Path rootDirPath = Files.createDirectory(fs.getPath("/foo"));

        final Path moduleList = rootDirPath.resolve("module-list.yaml");

        final Path moduleDef = moduleList.resolveSibling("sub/testmod.yaml");
        Files.createDirectories(moduleDef.getParent());

        // Create an empty moduleList in memory - tricks loader into loading Intrinsics
        try (final BufferedWriter w = Files.newBufferedWriter(moduleList)) {
            w.write("moduleBanks: [ {name: Intr, id: 0x05, defaultPackage: [com,example], moduleDefinitions: [ ] } ]");
        }

        final LoadableEntities le = new LoadableEntities(rootDirPath.toUri(), List.of(moduleList.getFileName().toString()), "txt", fs);

        // load it all, including the classpath Intrinsics
        final List<LoadedEntityScopes> loadedEntities = contextLoader.loadAndMap(le);

        // spot checks to make sure it's all there
        assertThat(loadedEntities).hasSize(1);
        final LoadedEntityScopes loadedEntityScopes = loadedEntities.get(0);
        assertThat(loadedEntityScopes.getRelativeOutputPath().toString()).isEqualTo("com/example/Intr.txt");
        assertThat(loadedEntityScopes.getScopes()).hasSize(2);
        assertThat(loadedEntityScopes.getScopes().get(0)).isInstanceOf(ConversionHelper.class);
        assertThat(loadedEntityScopes.getScopes().get(1)).isInstanceOf(Intrinsics.class);

        assertThat(((ConversionHelper) loadedEntityScopes.getScopes().get(0)).getAdditional()).containsEntry("package-elements", "com.example");
        assertThat(((Intrinsics) loadedEntityScopes.getScopes().get(1)).getZchars()).isNotNull().isNotEmpty();
    }

    @Test
    public void shouldFailWithNonexistentClasspathResource() throws URISyntaxException {
        final LoadableEntities le = new LoadableEntities(new URI("classpath:/"), singletonList("bar"), "java", fs);
        assertThatThrownBy(() -> contextLoader.loadAndMap(le)).isInstanceOf(IllegalArgumentException.class);
    }

    @Test
    public void shouldFailWithMissingFile() throws URISyntaxException {
        final LoadableEntities le = new LoadableEntities(new URI("file:/asd/"), singletonList("bar"), "java", fs);
        assertThatThrownBy(() -> contextLoader.loadAndMap(le)).isInstanceOf(UncheckedIOException.class).hasCauseInstanceOf(IOException.class);
    }
}
