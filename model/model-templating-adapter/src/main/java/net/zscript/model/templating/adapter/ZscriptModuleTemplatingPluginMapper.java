package net.zscript.model.templating.adapter;

import java.io.IOException;
import java.io.UncheckedIOException;
import java.net.URI;
import java.net.URL;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.List;
import java.util.stream.Stream;

import static java.util.Objects.requireNonNull;
import static java.util.stream.Collectors.toList;

import net.zscript.model.ZscriptModel;
import net.zscript.model.datamodel.ZscriptDataModel.ModuleModel;
import net.zscript.model.loader.ModelLoader;
import net.zscript.model.loader.ModuleBank;

public class ZscriptModuleTemplatingPluginMapper implements TemplatingPluginMapper {

    @Override
    public List<LoadableEntities.LoadedEntityContent> loadAndMap(LoadableEntities entities) {
        return entities.loadEntities(this::load);
    }

    private List<LoadableEntities.LoadedEntityContent> load(LoadableEntities.LoadableEntity entity) {
        List<LoadableEntities.LoadedEntityContent> result = new ArrayList<>();

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
                final String bankName = requireNonNull(moduleBank.getName(), "moduleBank name not defined");

                if (moduleBank.modules().isEmpty()) {
                    // if there are no modules, then this is an "intrinsics-only" template
                    final String           capitalizedBankName = Character.toUpperCase(bankName.charAt(0)) + bankName.substring(1) + "." + entity.getFileTypeSuffix();
                    final ConversionHelper helper              = new ConversionHelper();

                    helper.getAdditional().put("context-source", url);
                    final Path targetFileName = determineOutputPath(moduleBank.getDefaultPackage().stream(), capitalizedBankName, helper);

                    final List<Object> contents = List.of(helper, model.getIntrinsics());
                    result.add(entity.withContents(contents, targetFileName));
                } else {
                    for (final ModuleModel module : moduleBank.modules()) {
                        final String       moduleName      = requireNonNull(module.getName(), "module name not defined");
                        final List<String> packageElements = module.getPackage() != null ? module.getPackage() : moduleBank.getDefaultPackage();

                        ConversionHelper helper = new ConversionHelper();
                        helper.getAdditional().put("context-source", url);

                        final String capitalizedClassFilename = Character.toUpperCase(moduleName.charAt(0)) + moduleName.substring(1)
                                + "Module." + entity.getFileTypeSuffix();
                        final Path         targetFileName = determineOutputPath(Stream.concat(packageElements.stream(), Stream.of(bankName)), capitalizedClassFilename, helper);
                        final List<Object> contents       = List.of(helper, model.getIntrinsics(), module);
                        result.add(entity.withContents(contents, targetFileName));
                    }
                }
            }
        } catch (IOException ex) {
            throw new UncheckedIOException(ex);
        }
        return result;
    }

    private static Path determineOutputPath(Stream<String> packageElementStream, String fileName, ConversionHelper helper) {
        final List<String> fqcn = packageElementStream
                .map(p -> p.toLowerCase().replaceAll("- ", "_"))
                .collect(toList());
        helper.getAdditional().put("package-elements", String.join(".", fqcn));
        final Path packagePath = Path.of(fqcn.get(0), fqcn.subList(1, fqcn.size()).toArray(new String[0]));
        return packagePath.resolve(fileName);
    }
}
