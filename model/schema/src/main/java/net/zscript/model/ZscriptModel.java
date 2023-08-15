package net.zscript.model;

import static java.util.Comparator.comparing;
import static java.util.stream.Collectors.toList;

import java.io.IOException;
import java.io.UncheckedIOException;
import java.net.URL;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Optional;

import net.zscript.model.datamodel.IntrinsicsDataModel.Intrinsics;
import net.zscript.model.datamodel.ZscriptDataModel.ModuleModel;
import net.zscript.model.loader.ModelLoader;
import net.zscript.model.loader.ModuleBank;

public class ZscriptModel {
    private final Map<String, ModuleBank> moduleBanks = new HashMap<>();

    private final ModelLoader modelLoader;

    private ZscriptModel() {
        try {
            modelLoader = new ModelLoader();
        } catch (IOException e) {
            throw new UncheckedIOException(e);
        }
    }

    public static ZscriptModel rawModel() {
        return new ZscriptModel();
    }

    public static ZscriptModel standardModel() {
        ZscriptModel model = new ZscriptModel();
        try {
            Map<String, ModuleBank> banks = model.modelLoader.standardModel();
            model.addBanks(banks);
        } catch (IOException e) {
            throw new UncheckedIOException(e);
        }
        return model;
    }

    public ZscriptModel withModel(final URL moduleListLocation) {
        try {
            addBanks(modelLoader.loadModules(moduleListLocation));
        } catch (IOException e) {
            throw new UncheckedIOException(e);
        }
        return this;
    }

    public Intrinsics getIntrinsics() {
        return modelLoader.getIntrinsics();
    }

    public Optional<ModuleBank> getModuleBank(String moduleBankName) {
        return Optional.ofNullable(moduleBanks.get(moduleBankName));
    }

    public Optional<ModuleModel> getModule(String moduleBankName, String moduleName) {
        return getModuleBank(moduleBankName)
                .flatMap(mb -> mb.getModule(moduleName));
    }

    public List<ModuleBank> banks() {
        return moduleBanks.values().stream().sorted(comparing(ModuleBank::getId)).collect(toList());
    }

    private void addBanks(Map<String, ModuleBank> banks) {
        moduleBanks.putAll(banks);
    }
}
