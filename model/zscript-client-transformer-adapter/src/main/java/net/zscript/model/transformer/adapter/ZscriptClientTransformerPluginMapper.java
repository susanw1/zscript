package net.zscript.model.transformer.adapter;

import static java.util.Objects.requireNonNull;
import static java.util.stream.Collectors.toList;

import java.io.IOException;
import java.io.UncheckedIOException;
import java.net.URISyntaxException;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.List;
import java.util.stream.Stream;

import com.fasterxml.jackson.databind.MapperFeature;
import com.fasterxml.jackson.databind.json.JsonMapper;
import com.fasterxml.jackson.dataformat.yaml.YAMLFactory;
import com.fasterxml.jackson.module.mrbean.MrBeanModule;

import net.zscript.model.datamodel.ZscriptDataModel.ModuleModel;
import net.zscript.model.loader.ModelLoader;
import net.zscript.model.loader.ModuleBank;
import net.zscript.model.transformer.adapter.LoadableEntities.LoadableEntity;
import net.zscript.model.transformer.adapter.LoadableEntities.LoadedEntityContent;

public class ZscriptClientTransformerPluginMapper implements TransformerPluginMapper {

    @Override
    public List<LoadedEntityContent<ModuleModel>> loadAndMap(LoadableEntities entities) {
        return entities.loadEntities(e -> load(e));
    }

    private List<LoadedEntityContent<ModuleModel>> load(LoadableEntity entity) {
        List<LoadedEntityContent<ModuleModel>> result = new ArrayList<>();

        ModelLoader loader = ModelLoader.rawModel();
        try {
            ModelLoader model = loader.withModel(entity.getFullPath().toUri().toURL());
            for (ModuleBank moduleBank : model.getModuleBanks().values()) {
                for (ModuleModel module : moduleBank.modules()) {

                    final List<String> packageElements = module.getPackage() != null ? module.getPackage() : moduleBank.getDefaultPackage();
                    final String       bankName        = requireNonNull(module.getModuleBank().getName(), "moduleBank name not defined");
                    final String       moduleName      = requireNonNull(module.getName(), "module name not defined");

                    final List<String> fqcn = Stream.concat(packageElements.stream(), Stream.of(bankName))
                            .map(p -> p.toLowerCase().replace('-', '_'))
                            .collect(toList());

                    final Path   packagePath           = Path.of(fqcn.get(0), fqcn.subList(1, fqcn.size()).toArray(new String[0]));
                    final String capitalizedModuleName = Character.toUpperCase(moduleName.charAt(0)) + moduleName.substring(1) + "." + entity.getFileTypeSuffix();
                    final Path   moduleClassName       = packagePath.resolve(capitalizedModuleName);

                    result.add(entity.withContent(module, moduleClassName));
                }
            }
        } catch (IOException ex) {
            throw new UncheckedIOException(ex);
        } catch (URISyntaxException ex) {
            throw new RuntimeException(ex);
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
