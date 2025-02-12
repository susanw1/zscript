package net.zscript.model.datamodel;

import java.util.Collection;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.function.Function;
import java.util.regex.Pattern;

import static java.util.function.Function.identity;
import static java.util.stream.Collectors.counting;
import static java.util.stream.Collectors.groupingBy;
import static java.util.stream.Collectors.toList;

import net.zscript.model.ZscriptModel;
import net.zscript.model.datamodel.ZscriptDataModel.AnyTypeDefinition;
import net.zscript.model.datamodel.ZscriptDataModel.BitsetTypeDefinition;
import net.zscript.model.datamodel.ZscriptDataModel.CommandModel;
import net.zscript.model.datamodel.ZscriptDataModel.EnumTypeDefinition;
import net.zscript.model.datamodel.ZscriptDataModel.GenericField;
import net.zscript.model.datamodel.ZscriptDataModel.ModelComponent;
import net.zscript.model.datamodel.ZscriptDataModel.ModuleModel;
import net.zscript.model.datamodel.ZscriptDataModel.NotificationFieldModel;
import net.zscript.model.datamodel.ZscriptDataModel.NotificationModel;
import net.zscript.model.datamodel.ZscriptDataModel.NotificationSectionModel;
import net.zscript.model.datamodel.ZscriptDataModel.NotificationSectionNodeModel;
import net.zscript.model.datamodel.ZscriptDataModel.RequestFieldModel;
import net.zscript.model.datamodel.ZscriptDataModel.ResponseFieldModel;
import net.zscript.model.loader.ModuleBank;

public class ModelValidator {
    private final ZscriptModel model;

    public ModelValidator(ZscriptModel model) {
        this.model = model;
    }

    public void check() {
        for (ModuleBank bank : model.banks()) {
            if (!fitsBits(bank.getId(), 8)) {
                throw new ZscriptModelException("ModuleBank Id should be 0x00-0xff [bank=%s, id=0x%x]", bank.getName(), bank.getId());
            }
            if (!stringUpperCamel(bank.getName())) {
                throw new ZscriptModelException("ModuleBank names should be upper-camel-case, eg DeviceReset [bank=%s, id=0x%x]", bank.getName(), bank.getId());
            }

            if (bank.getDefaultPackage() != null) {
                for (String p : bank.getDefaultPackage()) {
                    if (!stringLowerCamel(p)) {
                        throw new ZscriptModelException("ModuleBank package fields should be lower-camel-case, eg 'i2cInterface' [bank=%s, id=0x%x]", bank.getName(), bank.getId());
                    }
                }
            }
            for (ModuleModel m : bank.modules()) {
                if (m.getModuleBank() != bank) {
                    throw new ZscriptModelException("Module must back-reference to its Bank [bank=%s, module=%s, back-ref=%s]", bank.getName(), m.getName(), m.getModuleBank());
                }
                if (m.getPackage() == null && bank.getDefaultPackage() == null) {
                    throw new ZscriptModelException("Module must define package if bank has no default [bank=%s, module=%s]", bank.getName(), m.getName());
                }
                checkModule(m);
            }
        }
    }

    void checkModule(ModuleModel module) {
        try {
            if (!stringUpperCamel(module.getName())) {
                throw new ZscriptModelException("Module names must be upper-camel-case, eg 'ThisAndThat' [module=%s, id=0x%x]", module.getName(), module.getId());
            }
            if (module.getName().matches(".*[Mm]odule")) {
                throw new ZscriptModelException("Module names must not end with 'Module' (we generally append that) [module=%s, id=0x%x]", module.getName(), module.getId());
            }
            if (!fitsBits(module.getId(), 4)) {
                throw new ZscriptModelException("Module Id must be 0x0-0xf [module=%s, id=0x%x]", module.getName(), module.getId());
            }
            if (module.getCommands() == null) {
                throw new ZscriptModelException("Module has no 'commands' section (must exist, even if empty) [module=%s, id=0x%x]", module.getName(), module.getId());
            }
            for (CommandModel c : module.getCommands()) {
                if (c.getModule() != module) {
                    throw new ZscriptModelException("Command must back-reference to its Module [module=%s, command=%s, back-ref=%s]", module.getName(), c.getName(), c.getModule());
                }
                checkCommand(c);
            }
            if (module.getNotifications() == null) {
                throw new ZscriptModelException("Module has no 'notifications' section (must exist, even if empty) [module=%s, id=0x%x]", module.getName(), module.getId());
            }
            for (NotificationModel n : module.getNotifications()) {
                if (n.getModule() != module) {
                    throw new ZscriptModelException("Notification definition must back-reference to its Module [module=%s, notification=%s, back-ref=%s]", module.getName(),
                            n.getName(), n.getModule());
                }
                checkNotification(n);
            }
        } catch (ZscriptModelException ex) {
            throw ex;
        } catch (RuntimeException ex) {
            throw new ZscriptModelException(ex, "Error while checking model [module=%s]", module.getName());
        }
    }

    void checkNotification(NotificationModel notification) {
        if (!stringLowerCamel(notification.getName())) {
            throw new ZscriptModelException("Notification names must be lower-camel-case, eg 'i2cInterface' [name=%s, notification=0x%x]", notification.getName(),
                    notification.getNotification());
        }
        if (notification.getName().matches(".*[Nn]otification")) {
            throw new ZscriptModelException("Notification names must not end with 'Notification' (we generally append that) [module=%s, id=0x%x]", notification.getName(),
                    notification.getNotification());
        }
        if (!fitsBits(notification.getNotification(), 4)) {
            throw new ZscriptModelException("Notification number must be 0x0-0xf [name=%s, notification=0x%x]", notification.getName(), notification.getNotification());
        }

        List<NotificationSectionNodeModel> sections = notification.getSections();
        if (sections == null || sections.isEmpty()) {
            throw new ZscriptModelException("Notification has no 'sections' [name=%s, notification=0x%x]", notification.getName(), notification.getNotification());
        }

        for (NotificationSectionNodeModel sNode : sections) {
            if (sNode.getSection() == null) {
                throw new ZscriptModelException("NotificationSectionNode must contain a 'section' definition [notification=%s]", notification.getNotificationName());
            }
            if (sNode.getNotification() != notification) {
                throw new ZscriptModelException("NotificationSectionNode must back-reference to its Notification [section=%s, notification=%s, back-ref=%s]",
                        sNode.getSection().getName(), notification.getName(), sNode.getNotification());
            }
            checkNotificationSection(sNode);
        }
    }

    void checkNotificationSection(NotificationSectionNodeModel sectionNode) {
        NotificationSectionModel section = sectionNode.getSection();
        if (!stringLowerCamel(section.getName())) {
            throw new ZscriptModelException("NotificationSection names must be lower-camel-case, eg 'pinChange' [name=%s, notification=0x%x]", section.getName(),
                    sectionNode.getNotification().getNotification());
        }

        checkFields(section, section.getFields(), "fields", NotificationFieldModel::getNotificationSection);
    }

    void checkCommand(CommandModel command) {
        if (!stringLowerCamel(command.getName())) {
            throw new ZscriptModelException("Command names must be lower-camel-case, eg 'i2cInterface' [name=%s, command=0x%x]", command.getName(), command.getCommand());
        }
        if (command.getName().matches(".*[Cc]ommand")) {
            throw new ZscriptModelException("Command names mustn't end with 'Command' (we generally append that) [name=%s, id=0x%x]", command.getName(), command.getCommand());
        }

        if (!fitsBits(command.getCommand(), 4)) {
            throw new ZscriptModelException("Command number must be 0x0-0xf [name=%s, command=0x%x]", command.getName(), command.getCommand());
        }

        checkFields(command, command.getRequestFields(), "requestFields", RequestFieldModel::getCommand);
        checkFields(command, command.getResponseFields(), "responseFields", ResponseFieldModel::getCommand);
    }

    private <F extends GenericField> void checkFields(ModelComponent fieldOwner, List<F> fields, String fieldTitle, Function<F, ModelComponent> backRef) {
        if (fields == null) {
            throw new ZscriptModelException("Command/notification has no '%s' section (must exist, even if empty) [name=%s]", fieldTitle, fieldOwner.getName());
        }
        for (F f : fields) {
            if (backRef.apply(f) != fieldOwner) {
                throw new ZscriptModelException("%s must back-reference to its parent [owner=%s, field=%s, back-ref=%s]", f.getClass().getSimpleName(), fieldOwner.getName(),
                        f.getName(), backRef.apply(f));
            }
            checkField(fieldOwner, f);
        }
        List<String> dupFieldNames = findDuplicates(fields, ModelComponent::getName);
        if (!dupFieldNames.isEmpty()) {
            throw new ZscriptModelException("Duplicate '%s' field name [name=%s, dupes=%s]", fieldTitle, fieldOwner.getName(), dupFieldNames);
        }
        List<String> dupFieldKeys = findDuplicates(fields, f -> String.valueOf(f.getKey()).toUpperCase());
        if (!dupFieldKeys.isEmpty()) {
            throw new ZscriptModelException("Duplicate '%s' field key [name=%s,  dupes=%s]", fieldTitle, fieldOwner.getName(), dupFieldKeys);
        }
    }

    void checkField(ModelComponent parent, GenericField field) {
        if (!stringLowerCamel(field.getName())) {
            throw new ZscriptModelException("Field names must be lower-camel-case, eg 'i2cInterface' [parent=%s, field=%s, key='%c']",
                    parent.getName(), field.getName(), field.getKey());
        }
        if (field.getName().matches("(set|get)[^a-z].*")) {
            throw new ZscriptModelException("Field names mustn't start with 'set' or 'get' (we generally prepend that) [parent=%s, field=%s, key='%c']",
                    parent.getName(), field.getName(), field.getKey());
        }

        if (field.getTypeDefinition() == null) {
            throw new ZscriptModelException("Field has no 'typeDefinition' section [parent=%s, field=%s, key='%c']",
                    parent.getName(), field.getName(), field.getKey());
        }

        if (!String.valueOf(field.getKey()).matches("[A-Z+\"]") && !(field.getKey() == 0 && field.getTypeDefinition() instanceof AnyTypeDefinition)) {
            throw new ZscriptModelException("Field key must be A-Z, '+' [parent=%s, field=%s, key='%c']",
                    parent.getName(), field.getName(), field.getKey());
        }

        if (field.getTypeDefinition() instanceof AnyTypeDefinition) {
            if (field.getKey() != 0) {
                throw new ZscriptModelException("Field with typeDefinition = 'any' must not have a key field [parent=%s, field=%s]",
                        parent.getName(), field.getName());
            }
        } else if (field.getTypeDefinition() instanceof BitsetTypeDefinition) {
            checkMultivalueTypeDefinition(parent, field, ((BitsetTypeDefinition) field.getTypeDefinition()).getBits(), ModelComponent::getName);
        } else if (field.getTypeDefinition() instanceof EnumTypeDefinition) {
            checkMultivalueTypeDefinition(parent, field, ((EnumTypeDefinition) field.getTypeDefinition()).getValues(), identity());
        }
    }

    <T> void checkMultivalueTypeDefinition(ModelComponent parent, GenericField field, List<T> items, Function<T, String> mapper) {
        if (items.size() > 16) {
            throw new ZscriptModelException("Field cannot have more that 16 field values [parent=%s, field=%s, key='%c']",
                    parent.getName(), field.getName(), field.getKey());
        }

        List<String> dupes = findDuplicates(items, mapper);
        if (!dupes.isEmpty()) {
            throw new ZscriptModelException("Field cannot have duplicate keys [parent=%s, field=%s, key='%c', dupes=%s]",
                    parent.getName(), field.getName(), field.getKey(), dupes);
        }
    }

    <T> List<String> findDuplicates(Collection<T> collection, Function<T, String> mapper) {
        if (collection.stream()
                .map(mapper)
                .anyMatch(Objects::isNull)) {
            throw new ZscriptModelException("Null item in collection [items=%s]", collection);
        }

        return collection.stream()
                .map(mapper)
                .collect(groupingBy(identity(), counting()))
                .entrySet().stream()
                .filter(e -> e.getValue() > 1)
                .map(Map.Entry::getKey).collect(toList());
    }

    private final Pattern LOWER_CAMEL = Pattern.compile("^\\p{Lower}\\p{Alnum}*$");

    boolean stringLowerCamel(String name) {
        return name != null && !name.isEmpty() && LOWER_CAMEL.matcher(name).matches();
    }

    private final Pattern UPPER_CAMEL = Pattern.compile("^\\p{Upper}\\p{Alnum}*$");

    boolean stringUpperCamel(String name) {
        return name != null && !name.isEmpty() && UPPER_CAMEL.matcher(name).matches();
    }

    boolean fitsBits(int value, int nBits) {
        return (value & -(1 << nBits)) == 0;
    }
}
