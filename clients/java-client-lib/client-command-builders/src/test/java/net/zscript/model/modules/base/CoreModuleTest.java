package net.zscript.model.modules.base;

import static net.zscript.model.modules.base.CoreModule.ReadIdCommand.Builder.IdType.TemporaryId;
import static org.assertj.core.api.Assertions.assertThat;
import static org.assertj.core.api.Assertions.assertThatThrownBy;

import org.junit.jupiter.api.Test;

import net.zscript.javaclient.commandbuilderapi.ZscriptMissingFieldException;
import net.zscript.javaclient.commandbuilderapi.nodes.ZscriptCommandNode;
import net.zscript.model.modules.base.CoreModule.ActivateCommand.ActivateResponse;
import net.zscript.model.modules.base.CoreModule.CapabilitiesCommand.CapabilitiesResponse;
import net.zscript.model.modules.base.CoreModule.EchoCommand.EchoResponse;
import net.zscript.model.modules.base.CoreModule.ReadIdCommand.ReadIdResponse;

public class CoreModuleTest {
    @Test
    public void shouldCreateCoreCapabilities() {
        ZscriptCommandNode<CapabilitiesResponse> c = CoreModule.capabilitiesBuilder()
                .build();
        assertThat(c.asStringUtf8()).isEqualTo("Z");
    }

    @Test
    public void shouldCreateCoreActivate() {
        ZscriptCommandNode<ActivateResponse> c = CoreModule.activateBuilder()
                .build();
        assertThat(c.asStringUtf8()).isEqualTo("Z2");
    }

    @Test
    public void shouldCreateCoreActivateWithField() {
        ZscriptCommandNode<ActivateResponse> c = CoreModule.ActivateCommand.builder()
                .setChallenge(3)
                .build();
        assertThat(c.asStringUtf8()).isEqualTo("Z2K3");
    }

    @Test
    public void shouldCreateCoreEcho() {
        ZscriptCommandNode<EchoResponse> c = CoreModule.echoBuilder()
                .setAny('J', 123)
                .build();
        assertThat(c.asStringUtf8()).isEqualTo("Z1J7b");
    }

    @Test
    public void shouldCreateCoreMatchCodeWithRequiredField() {
        ZscriptCommandNode<ReadIdResponse> c = CoreModule.readIdBuilder()
                .setIdType(TemporaryId)
                .setMatchId(new byte[] { 0x3a, 0x42 })
                .build();
        assertThat(c.asStringUtf8()).isEqualTo("Z4IM3a42");
    }

    @Test
    public void shouldCreateCoreMatchCodeWithoutRequiredField() {
        assertThatThrownBy(() -> CoreModule.readIdBuilder().setMatchId(new byte[] { 0x3a, 0x42 }).build()).isInstanceOf(ZscriptMissingFieldException.class)
                .hasMessage("missingKeys='I'");
    }

}
