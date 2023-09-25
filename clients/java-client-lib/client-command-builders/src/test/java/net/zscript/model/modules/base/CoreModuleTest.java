package net.zscript.model.modules.base;

import static net.zscript.model.modules.base.CoreModule.ReadIdCommandBuilder.IdType.TemporaryId;
import static org.assertj.core.api.Assertions.assertThat;
import static org.assertj.core.api.Assertions.assertThatThrownBy;

import org.junit.jupiter.api.Test;

import net.zscript.javaclient.commandbuilder.ZscriptCommandNode;
import net.zscript.javaclient.commandbuilder.ZscriptMissingFieldException;

public class CoreModuleTest {
    @Test
    public void shouldCreateCoreCapabilities() {
        ZscriptCommandNode c = CoreModule.capabilitiesBuilder()
                .build();
        byte[] ztext = c.compile();
        assertThat(ztext).containsExactly('Z');
    }

    @Test
    public void shouldCreateCoreActivate() {
        ZscriptCommandNode c = CoreModule.activateBuilder()
                .build();
        byte[] ztext = c.compile();
        assertThat(ztext).containsExactly('Z', '2');
    }

    @Test
    public void shouldCreateCoreActivateWithField() {
        ZscriptCommandNode c = CoreModule.activateBuilder()
                .setChallenge(3)
                .build();
        byte[] ztext = c.compile();
        assertThat(ztext).containsExactly('Z', '2', 'K', '3');
    }

    @Test
    public void shouldCreateCoreEcho() {
        ZscriptCommandNode c = CoreModule.echoBuilder()
                .setAny('J', 123)
                .build();
        byte[] ztext = c.compile();
        assertThat(ztext).containsExactly('Z', '1', 'J', '7', 'b');
    }

    @Test
    public void shouldCreateCoreMatchCodeWithRequiredField() {
        ZscriptCommandNode c = CoreModule.readIdBuilder()
                .setIdType(TemporaryId)
                .setMatchId(new byte[] { 0x3a, 0x42 })
                .build();
        byte[] ztext = c.compile();
        assertThat(ztext).containsExactly('Z', '4', 'I', '+', '3', 'a', '4', '2');
    }

    @Test
    public void shouldCreateCoreMatchCodeWithoutRequiredField() {
        assertThatThrownBy(() -> CoreModule.readIdBuilder().setMatchId(new byte[] { 0x3a, 0x42 }).build()).isInstanceOf(ZscriptMissingFieldException.class)
                .hasMessage("missingKeys='I'");
    }

    @Test
    public void should() {
        //        ZscriptCommandNode c = CoreModule.echoBuilder()EchoCommandBuilder.EchoCommandResponse.
        //                .setChallenge(3)
        //                .build();
        //        byte[] ztext = c.compile();
        //        assertThat(ztext).containsExactly('Z', '2', 'K', '3');
    }
}
