package net.zscript.model;

import java.util.Optional;

import static org.assertj.core.api.Assertions.assertThat;
import static org.assertj.core.api.Assertions.assertThatIllegalArgumentException;

import org.junit.jupiter.api.Test;

import net.zscript.model.datamodel.IntrinsicsDataModel.StatusModel;
import net.zscript.model.datamodel.ZscriptDataModel.CommandModel;
import net.zscript.model.datamodel.ZscriptDataModel.ModelComponent;
import net.zscript.model.datamodel.ZscriptDataModel.ModuleModel;

class ZscriptModelTest {
    @Test
    void shouldLoadRaw() {
        final ZscriptModel model = ZscriptModel.rawModel();
        assertThat(model.getIntrinsics().getStatus()).hasSize(33);
        assertThat(model.getIntrinsics().getRequestFields()).hasSize(1);
        assertThat(model.getIntrinsics().getResponseFields()).hasSize(1);
    }

    @Test
    void shouldLoadDefinition() {
        final ZscriptModel model = ZscriptModel.standardModel();
        assertThat(model.getModuleBank("Base")).isPresent();
        assertThat(model.getModuleBank("Banana")).isNotPresent();

        Optional<ModuleModel> module = model.getModuleBank("Base").orElseThrow().getModule("Testing");
        assertThat(module).isPresent();

        assertThat(model.getModule("Base", "Testing")).isPresent();
        assertThat(model.getModule("Nonexistent", "Testing")).isNotPresent();
        assertThat(model.getModule("Base", "Nonexistent")).isNotPresent();

        assertThat(module.get().getNotifications().get(2).getSections()).hasSize(2);
    }

    @Test
    void shouldFindCommands() {
        final ZscriptModel model = ZscriptModel.standardModel();
        assertThat(model.getCommand(0x0000)).isPresent();
        assertThat(model.getCommand(0x7b9d)).isPresent();
        assertThat(model.getCommand(0x1234)).isNotPresent();
        assertThatIllegalArgumentException().isThrownBy(() -> model.getCommand(0x10000))
                .withMessageContaining("valid 16 bit unsigned ints");
        assertThatIllegalArgumentException().isThrownBy(() -> model.getCommand(-1))
                .withMessageContaining("valid 16 bit unsigned ints");
    }

    @Test
    void shouldFindNotifications() {
        final ZscriptModel model = ZscriptModel.standardModel();
        assertThat(model.getNotification(0x0001)).isPresent();
        assertThat(model.getNotification(0x7b94)).isPresent();

        assertThat(model.getNotification(0x1234)).isNotPresent();
        assertThatIllegalArgumentException().isThrownBy(() -> model.getNotification(0x10000))
                .withMessageContaining("valid 16 bit unsigned ints");
        assertThatIllegalArgumentException().isThrownBy(() -> model.getNotification(-1))
                .withMessageContaining("valid 16 bit unsigned ints");
    }

    @Test
    public void shouldGetStatus() {
        final ZscriptModel model = ZscriptModel.rawModel();

        assertThat(model.getStatus(0)).isPresent().get()
                .extracting(StatusModel::getId, ModelComponent::getName)
                .containsExactly(0, "success");
        assertThat(model.getStatus(0x10)).isPresent().get()
                .extracting(StatusModel::getId, ModelComponent::getName)
                .containsExactly(0x10, "bufferOvrError");
        assertThat(model.getStatus(0xf)).isNotPresent();
    }

    @Test
    void shouldIdentifyFieldTypeDefinition() {
        final ZscriptModel           model   = ZscriptModel.standardModel();
        final Optional<CommandModel> command = model.getCommand(0x0001);
        assertThat(command).isPresent();

        assertThat(command.get().getRequestFields()).hasSize(3)
                .extracting(f -> f.getTypeDefinition().getType())
                .contains("number", "any", "cmd");
    }
}
