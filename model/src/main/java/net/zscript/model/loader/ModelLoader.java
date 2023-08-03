package net.zscript.model.loader;

import static java.util.Objects.requireNonNull;

import java.io.IOException;
import java.io.InputStream;
import java.io.UncheckedIOException;
import java.net.URL;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import com.fasterxml.jackson.databind.MapperFeature;
import com.fasterxml.jackson.databind.json.JsonMapper;
import com.fasterxml.jackson.dataformat.yaml.YAMLFactory;
import com.fasterxml.jackson.module.mrbean.MrBeanModule;

import net.zscript.model.datamodel.DefinitionResources;
import net.zscript.model.datamodel.DefinitionResources.ModuleBankDef;
import net.zscript.model.datamodel.IntrinsicsDataModel;
import net.zscript.model.datamodel.ZscriptDataModel;
import net.zscript.model.datamodel.ZscriptDataModel.CommandModel;
import net.zscript.model.datamodel.ZscriptDataModel.GenericParam;
import net.zscript.model.datamodel.ZscriptDataModel.ModuleModel;

public class ModelLoader {
    private final JsonMapper    jsonMapper;
    private IntrinsicsDataModel intrinsicsDataModel;

    private final Map<String, ModuleBank> moduleBanks = new HashMap<>();

    public static ModelLoader rawModel() {
        return new ModelLoader();
    }

    public static ModelLoader standardModel() {
        try {
            return new ModelLoader().withModel(ModelLoader.class.getResource("/datamodel/"));
        } catch (IOException e) {
            throw new UncheckedIOException(e);
        }
    }

    private ModelLoader() {
        jsonMapper = JsonMapper.builder(new YAMLFactory())
                .enable(MapperFeature.ACCEPT_CASE_INSENSITIVE_ENUMS)
                .addModule(new MrBeanModule())
                .build();

        final InputStream resourceStream = requireNonNull(getClass().getResourceAsStream("/intrinsics.yaml"), "intrinsics not found");
        try {
            intrinsicsDataModel = jsonMapper.readValue(resourceStream, IntrinsicsDataModel.class);
        } catch (IOException e) {
            throw new UncheckedIOException(e);
        }
    }

    public ModelLoader withModel(URL moduleListRoot) throws IOException {
        final URL moduleListLocation = new URL(moduleListRoot, "module-list.yaml");

        final DefinitionResources d;
        try (final InputStream openStream = moduleListLocation.openStream()) {
            d = jsonMapper.readValue(openStream, DefinitionResources.class);
        }

        for (ModuleBankDef mb : d.getModuleBanks()) {
            ModuleBank moduleBank = moduleBanks.computeIfAbsent(mb.getName(), n -> new ModuleBank(mb));

            for (String moduleDefinitionLocation : mb.getModuleDefinitions()) {
                URL              moduleDefinition = new URL(moduleListRoot, moduleDefinitionLocation);
                ZscriptDataModel model;
                try (final InputStream openStream = moduleDefinition.openStream()) {
                    model = jsonMapper.readValue(openStream, ZscriptDataModel.class);
                }
                for (ModuleModel mm : model.getModules()) {
                    mm.setModuleBank(moduleBank);
                    moduleBank.add(mm);

                    for (CommandModel c : mm.getCommands()) {
                        addIntrinsicIfRequired(requireNonNull(c.getRequestParams(), () -> c.getName() + ": requestParams null"),
                                intrinsicsDataModel.getIntrinsics().getRequestParams());
                        addIntrinsicIfRequired(requireNonNull(c.getResponseParams(), () -> c.getName() + ": responseParams null"),
                                intrinsicsDataModel.getIntrinsics().getResponseParams());
                    }
                }
            }

        }

        return this;
    }

    private <P extends GenericParam> void addIntrinsicIfRequired(List<P> commandParams, List<P> intrinsics) {
        for (P intrinsicParam : intrinsics) {
            for (P p : commandParams) {
                if (intrinsicParam.getLabel() == p.getLabel()) {
                    return;
                }
            }
            commandParams.add(intrinsicParam);
        }
    }

    public Map<String, ModuleBank> getModuleBanks() {
        return moduleBanks;
    }

}