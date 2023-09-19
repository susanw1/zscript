package net.zscript.model.loader;

import static java.util.Objects.requireNonNull;

import java.io.IOException;
import java.io.InputStream;
import java.net.URL;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import com.fasterxml.jackson.core.exc.StreamReadException;
import com.fasterxml.jackson.databind.DatabindException;
import com.fasterxml.jackson.databind.MapperFeature;
import com.fasterxml.jackson.databind.json.JsonMapper;
import com.fasterxml.jackson.dataformat.yaml.YAMLFactory;
import com.fasterxml.jackson.module.mrbean.MrBeanModule;

import net.zscript.model.datamodel.DefinitionResources;
import net.zscript.model.datamodel.DefinitionResources.ModuleBankDef;
import net.zscript.model.datamodel.IntrinsicsDataModel;
import net.zscript.model.datamodel.IntrinsicsDataModel.Intrinsics;
import net.zscript.model.datamodel.ZscriptDataModel.CommandModel;
import net.zscript.model.datamodel.ZscriptDataModel.GenericField;
import net.zscript.model.datamodel.ZscriptDataModel.ModuleModel;

/**
 * Loads and presents ModuleBanks model definitions.
 */
public class ModelLoader {
    public static final String ZSCRIPT_DATAMODEL_INTRINSICS_YAML = "/zscript-datamodel/intrinsics.yaml";

    private final JsonMapper          jsonMapper;
    private final IntrinsicsDataModel intrinsicsDataModel;

    public ModelLoader() throws IOException {
        jsonMapper = createJsonMapper();

        final InputStream resourceStream = requireNonNull(getClass().getResourceAsStream(ZSCRIPT_DATAMODEL_INTRINSICS_YAML), "intrinsics not found");
        intrinsicsDataModel = jsonMapper.readValue(resourceStream, IntrinsicsDataModel.class);
    }

    /**
     * This JSON mapper is defined to be able to correctly read Zscript configs - can be reused for tests etc.
     *
     * @return mapper that is used for parsing Zscript configs
     */
    public static JsonMapper createJsonMapper() {
        return JsonMapper.builder(new YAMLFactory())
                .enable(MapperFeature.ACCEPT_CASE_INSENSITIVE_ENUMS)
                .addModule(new MrBeanModule())
                .build();
    }

    public Map<String, ModuleBank> standardModel() throws IOException {
        return loadModules(ModelLoader.class.getResource("/zscript-datamodel/module-list.yaml"));
    }

    /**
     * @param moduleListLocation identifies the module-list inventory file to be opened (usually module-list.yaml).
     * @return the module banks described in the supplied module-list.yaml.
     * @throws IOException fails to load or decode the module definition
     */
    public Map<String, ModuleBank> loadModules(final URL moduleListLocation) throws IOException {
        final Map<String, ModuleBank> moduleBanks = new HashMap<>();

        final DefinitionResources d = jsonMapper.readValue(moduleListLocation, DefinitionResources.class);

        for (final ModuleBankDef mb : d.getModuleBanks()) {
            final ModuleBank moduleBank = moduleBanks.computeIfAbsent(mb.getName(), n -> new ModuleBank(mb));

            for (final String moduleDefinitionLocation : mb.getModuleDefinitions()) {
                final URL moduleDefinition = new URL(moduleListLocation, moduleDefinitionLocation);

                final ModuleModel moduleModel;
                try (final InputStream openStream = moduleDefinition.openStream()) {
                    moduleModel = jsonMapper.readValue(openStream, ModuleModel.class);
                }
                moduleModel.setModuleBank(moduleBank);
                moduleBank.add(moduleModel);

                for (final CommandModel c : moduleModel.getCommands()) {
                    addIntrinsicIfRequired(requireNonNull(c.getRequestFields(), () -> c.getName() + ": requestFields null"),
                            intrinsicsDataModel.getIntrinsics().getRequestFields());
                    addIntrinsicIfRequired(requireNonNull(c.getResponseFields(), () -> c.getName() + ": responseFields null"),
                            intrinsicsDataModel.getIntrinsics().getResponseFields());
                }
            }
        }

        return moduleBanks;
    }

    private <F extends GenericField> void addIntrinsicIfRequired(final List<F> fields, final List<F> intrinsics) {
        for (final F intrinsicField : intrinsics) {
            for (final F f : fields) {
                if (intrinsicField.getKey() == f.getKey()) {
                    return;
                }
            }
            fields.add(intrinsicField);
        }
    }

    public Intrinsics getIntrinsics() {
        return intrinsicsDataModel.getIntrinsics();
    }
}
