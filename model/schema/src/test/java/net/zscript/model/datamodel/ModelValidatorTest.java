package net.zscript.model.datamodel;

import java.util.Arrays;
import java.util.List;
import java.util.function.Supplier;

import static java.util.Arrays.asList;
import static java.util.Arrays.fill;
import static net.zscript.model.ZscriptModel.standardModel;
import static org.assertj.core.api.Assertions.assertThat;
import static org.assertj.core.api.Assertions.assertThatThrownBy;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;

import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

import net.zscript.model.ZscriptModel;
import net.zscript.model.datamodel.ZscriptDataModel.AnyTypeDefinition;
import net.zscript.model.datamodel.ZscriptDataModel.BitsetTypeDefinition;
import net.zscript.model.datamodel.ZscriptDataModel.BitsetTypeDefinition.Bit;
import net.zscript.model.datamodel.ZscriptDataModel.CommandModel;
import net.zscript.model.datamodel.ZscriptDataModel.EnumTypeDefinition;
import net.zscript.model.datamodel.ZscriptDataModel.ModuleModel;
import net.zscript.model.datamodel.ZscriptDataModel.NotificationFieldModel;
import net.zscript.model.datamodel.ZscriptDataModel.NotificationModel;
import net.zscript.model.datamodel.ZscriptDataModel.NotificationSectionModel;
import net.zscript.model.datamodel.ZscriptDataModel.NotificationSectionNodeModel;
import net.zscript.model.datamodel.ZscriptDataModel.RequestFieldModel;
import net.zscript.model.datamodel.ZscriptDataModel.ResponseFieldModel;
import net.zscript.model.datamodel.ZscriptDataModel.TextTypeDefinition;
import net.zscript.model.loader.ModuleBank;

class ModelValidatorTest {
    private ModuleBank                   mockModuleBank;
    private ModuleModel                  mockModule;
    private CommandModel                 mockCommand;
    private NotificationSectionModel     mockNotifSection;
    private NotificationModel            mockNotif;
    private NotificationSectionNodeModel mockNotifNodeSec;
    private RequestFieldModel            mockReqBsField;
    private ResponseFieldModel           mockRespEnumField;
    private NotificationFieldModel       mockNotifAnyField;
    private TextTypeDefinition           mockTypeDefn;

    @BeforeEach
    public void setup() {
        // This creates one of every level of entity, provides valid initial values, and wires them up
        mockModuleBank = mock(ModuleBank.class);
        when(mockModuleBank.getId()).thenReturn(255);
        when(mockModuleBank.getName()).thenReturn("Test");
        when(mockModuleBank.getDefaultPackage()).thenReturn(List.of("abc", "def"));

        mockModule = mock(ModuleModel.class);
        when(mockModuleBank.modules()).thenReturn(List.of(mockModule));
        when(mockModule.getModuleBank()).thenReturn(mockModuleBank);
        when(mockModule.getId()).thenReturn((short) 15);
        when(mockModule.getName()).thenReturn("Moddy");
        when(mockModule.getPackage()).thenReturn(List.of("xyz", "wxy"));

        mockCommand = mock(CommandModel.class);
        when(mockModule.getCommands()).thenReturn(List.of(mockCommand));
        when(mockCommand.getModule()).thenReturn(mockModule);
        when(mockCommand.getCommand()).thenReturn((byte) 15);
        when(mockCommand.getName()).thenReturn("cmd");

        mockNotif = mock(NotificationModel.class);
        when(mockModule.getNotifications()).thenReturn(List.of(mockNotif));
        when(mockNotif.getModule()).thenReturn(mockModule);
        when(mockNotif.getNotification()).thenReturn((byte) 15);
        when(mockNotif.getName()).thenReturn("notif");

        mockNotifNodeSec = mock(NotificationSectionNodeModel.class);
        when(mockNotif.getSections()).thenReturn(List.of(mockNotifNodeSec));
        when(mockNotifNodeSec.getNotification()).thenReturn(mockNotif);

        mockNotifSection = mock(NotificationSectionModel.class);
        when(mockNotifNodeSec.getSection()).thenReturn(mockNotifSection);
        when(mockNotifSection.getName()).thenReturn("notifSec");

        mockReqBsField = mock(RequestFieldModel.class);
        when(mockCommand.getRequestFields()).thenReturn(List.of(mockReqBsField));
        when(mockReqBsField.getCommand()).thenReturn(mockCommand);
        when(mockReqBsField.getKey()).thenReturn('P');
        when(mockReqBsField.getName()).thenReturn("field1");
        when(mockReqBsField.getTypeDefinition()).thenReturn(mock(BitsetTypeDefinition.class));

        mockRespEnumField = mock(ResponseFieldModel.class);
        when(mockCommand.getResponseFields()).thenReturn(List.of(mockRespEnumField));
        when(mockRespEnumField.getCommand()).thenReturn(mockCommand);
        when(mockRespEnumField.getKey()).thenReturn('Q');
        when(mockRespEnumField.getName()).thenReturn("field2");
        when(mockRespEnumField.getTypeDefinition()).thenReturn(mock(EnumTypeDefinition.class));

        mockNotifAnyField = mock(NotificationFieldModel.class);
        when(mockNotifSection.getFields()).thenReturn(List.of(mockNotifAnyField));
        when(mockNotifAnyField.getNotificationSection()).thenReturn(mockNotifSection);
        when(mockNotifAnyField.getKey()).thenReturn('\0');
        when(mockNotifAnyField.getName()).thenReturn("fieldS");
        when(mockNotifAnyField.getTypeDefinition()).thenReturn(mock(AnyTypeDefinition.class));
    }

    @Test
    void shouldCheck() {
        final ZscriptModel model = standardModel();
        new ModelValidator(model).check();
    }

    @Test
    void shouldCheckBitRange() {
        final ModelValidator modelValidator = new ModelValidator(standardModel());
        for (int i = 0; i < 16; i++) {
            assertThat(modelValidator.fitsBits(i, 4)).isTrue();
        }
        assertThat(modelValidator.fitsBits(-1, 4)).isFalse();
        assertThat(modelValidator.fitsBits(16, 4)).isFalse();
    }

    @Test
    public void shouldRejectBadModuleBanks() {
        final ZscriptModel mockModel = mock(ZscriptModel.class);
        when(mockModel.banks()).thenReturn(List.of(mockModuleBank));

        final ModelValidator modelValidator = new ModelValidator(mockModel);
        modelValidator.check();

        checkAlternateValue(modelValidator::check, mockModuleBank::getDefaultPackage, List.of("abc", "Def"), "ModuleBank package fields should be lower-camel-case");
        checkAlternateValue(modelValidator::check, mockModuleBank::getName, "bar", "ModuleBank names should be upper-camel-case");
        checkAlternateValue(modelValidator::check, mockModuleBank::getId, 256, "ModuleBank Id should be 0x00-0xff");
    }

    @Test
    public void shouldRejectBadModuleBankModules() {
        final ZscriptModel mockModel = mock(ZscriptModel.class);
        when(mockModel.banks()).thenReturn(List.of(mockModuleBank));

        final ModelValidator modelValidator = new ModelValidator(mockModel);
        modelValidator.check();

        checkAlternateValue(modelValidator::check, mockModule::getId, (short) 16, "Module Id must be 0x0-0xf");
        checkAlternateValue(modelValidator::check, mockModule::getCommands, null, "Module has no 'commands' section");
        checkAlternateValue(modelValidator::check, mockModule::getName, "TestModule", "Module names must not end with 'Module'");
        checkAlternateValue(modelValidator::check, mockModule::getName, "foo", "Module names must be upper-camel-case");
        checkAlternateValue(modelValidator::check, mockModule::getModuleBank, null, "Module must back-reference to its Bank");

        when(mockModuleBank.getDefaultPackage()).thenReturn(null);
        checkAlternateValue(modelValidator::check, mockModule::getPackage, null, "Module must define package if bank has no default");
    }

    @Test
    public void shouldRejectBadModuleCommands() {
        final ZscriptModel   mockModel      = mock(ZscriptModel.class);
        final ModelValidator modelValidator = new ModelValidator(mockModel);
        modelValidator.checkModule(mockModule);

        checkAlternateValue(() -> modelValidator.checkModule(mockModule), mockCommand::getCommand, (byte) 16, "Command number must be 0x0-0xf");
        checkAlternateValue(() -> modelValidator.checkModule(mockModule), mockCommand::getName, "testCommand", "Command names mustn't end with 'Command'");
        checkAlternateValue(() -> modelValidator.checkModule(mockModule), mockCommand::getName, "Foo", "Command names must be lower-camel-case");
        checkAlternateValue(() -> modelValidator.checkModule(mockModule), mockCommand::getModule, null, "Command must back-reference to its Module");
    }

    @Test
    public void shouldRejectBadModuleNotifications() {
        final ZscriptModel   mockModel      = mock(ZscriptModel.class);
        final ModelValidator modelValidator = new ModelValidator(mockModel);
        modelValidator.checkModule(mockModule);

        checkAlternateValue(() -> modelValidator.checkModule(mockModule), mockModule::getNotifications, null, "Module has no 'notifications' section");
        checkAlternateValue(() -> modelValidator.checkModule(mockModule), mockNotif::getModule, null, "Notification definition must back-reference to its Module");
    }

    @Test
    public void shouldRejectBadNotifications() {
        final ZscriptModel   mockModel      = mock(ZscriptModel.class);
        final ModelValidator modelValidator = new ModelValidator(mockModel);
        modelValidator.checkNotification(mockNotif);

        final Runnable testAction = () -> modelValidator.checkNotification(mockNotif);
        checkAlternateValue(testAction, mockNotif::getNotification, (byte) 16, "Notification number must be 0x0-0xf");
        checkAlternateValue(testAction, mockNotif::getName, "testNotification", "Notification names must not end with 'Notification'");
        checkAlternateValue(testAction, mockNotif::getName, "Foo", "Notification names must be lower-camel-case");

        checkAlternateValue(testAction, mockNotif::getSections, null, "Notification has no 'sections'");
        checkAlternateValue(testAction, mockNotif::getSections, List.of(), "Notification has no 'sections'");

        checkAlternateValue(testAction, mockNotifNodeSec::getSection, null,
                "NotificationSectionNode must contain a 'section' definition");
        checkAlternateValue(testAction, mockNotifNodeSec::getNotification, null,
                "NotificationSectionNode must back-reference to its Notification");
    }

    @Test
    public void shouldRejectBadNotificationSections() {
        final ZscriptModel   mockModel      = mock(ZscriptModel.class);
        final ModelValidator modelValidator = new ModelValidator(mockModel);
        modelValidator.checkNotificationSection(mockNotifNodeSec);

        final Runnable testAction = () -> modelValidator.checkNotificationSection(mockNotifNodeSec);
        checkAlternateValue(testAction, mockNotifSection::getName, "Bar", "NotificationSection names must be lower-camel-case");
        checkAlternateValue(testAction, mockNotifSection::getFields, null, "Command/notification has no 'fields' section");
        checkAlternateValue(testAction, mockNotifAnyField::getNotificationSection, null, "must back-reference to its parent");
        checkAlternateValue(testAction, mockNotifSection::getFields, List.of(mockNotifAnyField, mockNotifAnyField), "Duplicate 'fields' field name");
    }

    @Test
    public void shouldRejectBadFields() {
        final ZscriptModel   mockModel      = mock(ZscriptModel.class);
        final ModelValidator modelValidator = new ModelValidator(mockModel);
        modelValidator.checkField(mockNotifSection, mockNotifAnyField);
        modelValidator.checkField(mockCommand, mockReqBsField);
        modelValidator.checkField(mockCommand, mockRespEnumField);

        final Runnable testReqAction = () -> modelValidator.checkField(mockCommand, mockReqBsField);
        checkAlternateValue(testReqAction, mockReqBsField::getName, "Foo", "Field names must be lower-camel-case");
        checkAlternateValue(testReqAction, mockReqBsField::getName, "setThing", "Field names mustn't start with 'set' or 'get'");
        checkAlternateValue(testReqAction, mockReqBsField::getKey, 'a', "Field key must be A-Z, '+'");
        checkAlternateValue(testReqAction, mockReqBsField::getKey, '\0', "Field key must be A-Z, '+'");
        checkAlternateValue(testReqAction, mockReqBsField::getTypeDefinition, null, "Field has no 'typeDefinition' section");

        final Bit b1 = mock(Bit.class);
        when(b1.getName()).thenReturn("b1");

        final Bit[] bits = new Bit[17];
        fill(bits, b1);
        checkAlternateValue(testReqAction, () -> ((BitsetTypeDefinition) mockReqBsField.getTypeDefinition()).getBits(), List.of(b1, b1), "Field cannot have duplicate keys");
        checkAlternateValue(testReqAction, () -> ((BitsetTypeDefinition) mockReqBsField.getTypeDefinition()).getBits(), asList(bits),
                "Field cannot have more that 16 field values");

        List<String>   values         = Arrays.asList("a", null, "c");
        final Runnable testRespAction = () -> modelValidator.checkField(mockCommand, mockRespEnumField);
        checkAlternateValue(testRespAction, () -> ((EnumTypeDefinition) mockRespEnumField.getTypeDefinition()).getValues(), values, "Null item in collection");

        checkAlternateValue(() -> modelValidator.checkField(mockCommand, mockNotifAnyField), mockNotifAnyField::getKey, 'X',
                "Field with typeDefinition = 'any' must not have a key field");
    }

    /** Utility to modify a field, run a test action, and then put it back */
    <T> void checkAlternateValue(Runnable testAction, Supplier<T> mockOp, T testVal, String msg) {
        final T old = mockOp.get();
        when(mockOp.get()).thenReturn(testVal);
        assertThatThrownBy(testAction::run).isInstanceOf(ZscriptModelException.class)
                .hasMessageContaining(msg);
        when(mockOp.get()).thenReturn(old);
    }

    @Test
    void shouldCheckIsLower() {
        final ModelValidator modelValidator = new ModelValidator(standardModel());
        assertThat(modelValidator.stringLowerCamel("a")).isTrue();
        assertThat(modelValidator.stringLowerCamel("aB")).isTrue();
        assertThat(modelValidator.stringLowerCamel("dogFood")).isTrue();
        assertThat(modelValidator.stringLowerCamel("dog2Food")).isTrue();

        assertThat(modelValidator.stringLowerCamel("")).isFalse();
        assertThat(modelValidator.stringLowerCamel(null)).isFalse();
        assertThat(modelValidator.stringLowerCamel("A")).isFalse();
        assertThat(modelValidator.stringLowerCamel("AB")).isFalse();
        assertThat(modelValidator.stringLowerCamel("DogFood")).isFalse();
        assertThat(modelValidator.stringLowerCamel("dog_food")).isFalse();
    }

    @Test
    void shouldCheckIsUpper() {
        final ModelValidator modelValidator = new ModelValidator(standardModel());
        assertThat(modelValidator.stringUpperCamel("A")).isTrue();
        assertThat(modelValidator.stringUpperCamel("AB")).isTrue();
        assertThat(modelValidator.stringUpperCamel("DogFood")).isTrue();
        assertThat(modelValidator.stringUpperCamel("Dog2Food")).isTrue();

        assertThat(modelValidator.stringUpperCamel("")).isFalse();
        assertThat(modelValidator.stringUpperCamel(null)).isFalse();
        assertThat(modelValidator.stringUpperCamel("a")).isFalse();
        assertThat(modelValidator.stringUpperCamel("aB")).isFalse();
        assertThat(modelValidator.stringUpperCamel("dogFood")).isFalse();
        assertThat(modelValidator.stringUpperCamel("dog_food")).isFalse();
        assertThat(modelValidator.stringUpperCamel("Dog_food")).isFalse();
    }

    @Test
    void shouldFindDupes() {
        final ModelValidator modelValidator = new ModelValidator(standardModel());
        assertThat(modelValidator.findDuplicates(List.of(1, 5, 2, 5, 6, 3, 7, 1, 9), String::valueOf)).containsExactlyInAnyOrder("1", "5");
        assertThat(modelValidator.findDuplicates(List.of(), String::valueOf)).isEmpty();
        assertThat(modelValidator.findDuplicates(List.of(3), String::valueOf)).isEmpty();
        assertThat(modelValidator.findDuplicates(List.of(3, 3, 3), String::valueOf)).containsExactly("3");
    }
}
