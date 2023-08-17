package net.zscript.model.transformer.adapter;

import static java.util.Objects.requireNonNull;
import static java.util.stream.Collectors.toList;

import java.io.IOException;
import java.io.UncheckedIOException;
import java.net.URI;
import java.net.URL;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.List;
import java.util.stream.Stream;

import com.fasterxml.jackson.databind.MapperFeature;
import com.fasterxml.jackson.databind.json.JsonMapper;
import com.fasterxml.jackson.dataformat.yaml.YAMLFactory;
import com.fasterxml.jackson.module.mrbean.MrBeanModule;

import net.zscript.model.ZscriptModel;
import net.zscript.model.datamodel.ZscriptDataModel.ModuleModel;
import net.zscript.model.loader.ModelLoader;
import net.zscript.model.loader.ModuleBank;
import net.zscript.model.transformer.adapter.LoadableEntities.LoadableEntity;
import net.zscript.model.transformer.adapter.LoadableEntities.LoadedEntityContent;

public class ZscriptClientTransformerPluginMapper implements TransformerPluginMapper {

    @Override
    public List<LoadedEntityContent> loadAndMap(LoadableEntities entities) {
        return entities.loadEntities(e -> load(e));
    }

    private List<LoadedEntityContent> load(LoadableEntity entity) {
        List<LoadedEntityContent> result = new ArrayList<>();

        ConversionUtils utils = new ConversionUtils();

        try {
            final URI fullPath = entity.getFullPath();
            final URL url;
            if ("classpath".equals(fullPath.getScheme())) {
                url = ModelLoader.class.getResource(fullPath.getPath());
            } else {
                url = fullPath.toURL();
            }

            final ZscriptModel model = ZscriptModel.rawModel().withModel(url);

            for (final ModuleBank moduleBank : model.banks()) {
                for (final ModuleModel module : moduleBank.modules()) {

                    final List<String> packageElements = module.getPackage() != null ? module.getPackage() : moduleBank.getDefaultPackage();

                    final String bankName   = requireNonNull(module.getModuleBank().getName(), "moduleBank name not defined");
                    final String moduleName = requireNonNull(module.getName(), "module name not defined");

                    final List<String> fqcn = Stream.concat(packageElements.stream(), Stream.of(bankName))
                            .map(p -> p.toLowerCase().replace('-', '_'))
                            .collect(toList());

                    utils.getMapOfStuff().put("package-elements", String.join(".", fqcn));

                    final Path   packagePath           = Path.of(fqcn.get(0), fqcn.subList(1, fqcn.size()).toArray(new String[0]));
                    final String capitalizedModuleName = Character.toUpperCase(moduleName.charAt(0)) + moduleName.substring(1) + "Module." + entity.getFileTypeSuffix();
                    final Path   moduleClassFileName   = packagePath.resolve(capitalizedModuleName);

                    final List<Object> contents = List.of(utils, model.getIntrinsics(), module);
                    result.add(entity.withContents(contents, moduleClassFileName));
                }
            }
        } catch (IOException ex) {
            throw new UncheckedIOException(ex);
        }
        return result;
    }

    public static JsonMapper createJsonMapper() {
        return JsonMapper.builder(new YAMLFactory())
                .enable(MapperFeature.ACCEPT_CASE_INSENSITIVE_ENUMS)
                .addModule(new MrBeanModule())
                .build();
    }
}
