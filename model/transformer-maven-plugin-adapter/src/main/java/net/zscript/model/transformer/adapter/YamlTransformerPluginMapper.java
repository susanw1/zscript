package net.zscript.model.transformer.adapter;

import java.io.IOException;
import java.io.UncheckedIOException;
import java.nio.file.Path;
import java.util.List;
import java.util.Map;

import com.fasterxml.jackson.databind.MapperFeature;
import com.fasterxml.jackson.databind.json.JsonMapper;
import com.fasterxml.jackson.dataformat.yaml.YAMLFactory;
import com.fasterxml.jackson.module.mrbean.MrBeanModule;

import net.zscript.model.transformer.adapter.LoadableEntities.LoadableEntity;
import net.zscript.model.transformer.adapter.LoadableEntities.LoadedEntityContent;

public class YamlTransformerPluginMapper implements TransformerPluginMapper {
    private final JsonMapper jsonMapper = createJsonMapper();

    @Override
    public List<LoadedEntityContent<Map<?, ?>>> loadAndMap(LoadableEntities entities) {
        return entities.loadEntities(e -> load(e));
    }

    private LoadedEntityContent<Map<?, ?>> load(LoadableEntity entity) {
        try {
            final String relativePathToSource = entity.getRelativePath();
            final Path   fullPathToSource     = Path.of(entity.getRootPathName()).resolve(relativePathToSource);

            Path relativePathToOutput = findRelativePathToOutput(relativePathToSource, entity.getFileTypeSuffix());

            final Map<?, ?> value = jsonMapper.reader().readValue(fullPathToSource.toFile(), Map.class);
            return entity.withContent(value, relativePathToOutput.toString());
        } catch (IOException ex) {
            throw new UncheckedIOException(ex);
        }
    }

    public static JsonMapper createJsonMapper() {
        return JsonMapper.builder(new YAMLFactory())
                .enable(MapperFeature.ACCEPT_CASE_INSENSITIVE_ENUMS)
                .addModule(new MrBeanModule())
                .build();
    }
}
