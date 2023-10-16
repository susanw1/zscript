package net.zscript.model.modules.base;

import static net.zscript.model.modules.base.CoreModule.ReadIdCommand.Builder.IdType.TemporaryId;
import static org.assertj.core.api.Assertions.assertThat;
import static org.assertj.core.api.Assertions.assertThatThrownBy;

import org.junit.jupiter.api.Test;

import net.zscript.javaclient.commandbuilder.commandnodes.ZscriptCommandNode;
import net.zscript.javaclient.commandbuilder.ZscriptMissingFieldException;

public class CoreModuleTest {
    @Test
    public void shouldCreateCoreCapabilities() {
        ZscriptCommandNode c = CoreModule.capabilitiesBuilder()
                .build();
        String ztext = c.asString();
        assertThat(ztext).isEqualTo("Z");
    }

    @Test
    public void shouldCreateCoreActivate() {
        ZscriptCommandNode c = CoreModule.activateBuilder()
                .build();
        String ztext = c.asString();
        assertThat(ztext).isEqualTo("Z2");
    }

    @Test
    public void shouldCreateCoreActivateWithField() {
        ZscriptCommandNode c = CoreModule.ActivateCommand.builder()
                .setChallenge(3)
                .build();
        String ztext = c.asString();
        assertThat(ztext).isEqualTo("K3Z2");
    }

    @Test
    public void shouldCreateCoreEcho() {
        ZscriptCommandNode c = CoreModule.echoBuilder()
                .setAny('J', 123)
                .build();
        String ztext = c.asString();
        assertThat(ztext).isEqualTo("J7bZ1");
    }

    @Test
    public void shouldCreateCoreMatchCodeWithRequiredField() {
        ZscriptCommandNode c = CoreModule.readIdBuilder()
                .setIdType(TemporaryId)
                .setMatchId(new byte[] { 0x3a, 0x42 })
                .build();
        String ztext = c.asString();
        assertThat(ztext).isEqualTo("IZ4+3a42");
    }

    @Test
    public void shouldCreateCoreMatchCodeWithoutRequiredField() {
        assertThatThrownBy(() -> CoreModule.readIdBuilder().setMatchId(new byte[] { 0x3a, 0x42 }).build()).isInstanceOf(ZscriptMissingFieldException.class)
                .hasMessage("missingKeys='I'");
    }

}
