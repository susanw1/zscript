package net.zscript.model;

import java.io.IOException;
import java.io.UncheckedIOException;
import java.net.URL;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Optional;

import static java.util.Comparator.comparing;
import static java.util.stream.Collectors.toList;

import com.fasterxml.jackson.annotation.JsonProperty;

import net.zscript.model.datamodel.IntrinsicsDataModel.Intrinsics;
import net.zscript.model.datamodel.ModelValidator;
import net.zscript.model.datamodel.ZscriptDataModel;
import net.zscript.model.datamodel.ZscriptDataModel.ModuleModel;
import net.zscript.model.loader.ModelLoader;
import net.zscript.model.loader.ModuleBank;

public class ZscriptModel {
    private final Map<String, ModuleBank>  moduleBanks     = new HashMap<>();
    private final Map<Integer, ModuleBank> moduleBanksById = new HashMap<>();

    private final ModelLoader modelLoader;

    private final ModelValidator validator;

    private ZscriptModel() {
        try {
            modelLoader = new ModelLoader();
            validator = new ModelValidator(this);
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
        model.validate();
        return model;
    }

    private void validate() {
        validator.check();
    }

    public ZscriptModel withModel(final URL moduleListLocation) {
        try {
            addBanks(modelLoader.loadModules(moduleListLocation));
        } catch (IOException e) {
            throw new UncheckedIOException(e);
        }
        validate();
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

    public Optional<ZscriptDataModel.CommandModel> getCommand(int id) {
        if (id < 0 || id > 0xFFFF) {
            throw new IllegalArgumentException("Command IDs must be valid 16 bit unsigned ints");
        }
        return Optional.ofNullable(moduleBanksById.get((id >> 8) & 0xFF))
                .flatMap(b -> b.getModule((id >> 4) & 0xF))
                .flatMap(m -> m.getCommandById((id & 0xF)));
    }

    public Optional<ZscriptDataModel.NotificationModel> getNotification(int id) {
        if (id < 0 || id > 0xFFFF) {
            throw new IllegalArgumentException("Command IDs must be valid 16 bit unsigned ints");
        }
        return Optional.ofNullable(moduleBanksById.get((id >> 8) & 0xFF))
                .flatMap(b -> b.getModule((id >> 4) & 0xF))
                .flatMap(m -> m.getNotificationById((id & 0xF)));
    }

    public Optional<ZscriptDataModel.StatusModel> getStatus(int value) {
        Intrinsics intrinsics = getIntrinsics();
        for (ZscriptDataModel.StatusModel status : intrinsics.getStatus()) {
            if (status.getId() == value) {
                return Optional.of(status);
            } else if (status.getId() > value) {
                break;
            }
        }
        return Optional.empty();
    }

    @JsonProperty
    public List<ModuleBank> banks() {
        return moduleBanks.values().stream().sorted(comparing(ModuleBank::getId)).collect(toList());
    }

    private void addBanks(Map<String, ModuleBank> banks) {
        moduleBanks.putAll(banks);
        for (ModuleBank bank : banks.values()) {
            moduleBanksById.put(bank.getId(), bank);
        }
    }
}
