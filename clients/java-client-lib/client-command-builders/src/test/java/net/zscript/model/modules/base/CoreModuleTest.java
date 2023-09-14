package net.zscript.model.modules.base;

import static org.assertj.core.api.Assertions.assertThat;
import static org.assertj.core.api.Assertions.assertThatThrownBy;

import org.junit.jupiter.api.Test;

import net.zscript.javaclient.commandbuilder.ZscriptCommandNode;
import net.zscript.javaclient.commandbuilder.ZscriptMissingFieldException;

public class CoreModuleTest {
    @Test
    public void shouldCreateCoreCapabilities() {
        ZscriptCommandNode c = CoreModule.capabilities()
                .build();
        byte[] ztext = c.compile();
        assertThat(ztext).containsExactly('Z');
    }

    @Test
    public void shouldCreateCoreActivate() {
        ZscriptCommandNode c = CoreModule.activate()
                .build();
        byte[] ztext = c.compile();
        assertThat(ztext).containsExactly('Z', '2');
    }

    @Test
    public void shouldCreateCoreActivateWithField() {
        ZscriptCommandNode c = CoreModule.activate()
                .setChallenge(3)
                .build();
        byte[] ztext = c.compile();
        assertThat(ztext).containsExactly('Z', '2', 'K', '3');
    }

    @Test
    public void shouldCreateCoreEcho() {
        ZscriptCommandNode c = CoreModule.echo()
                .setAny('J', 123)
                .build();
        byte[] ztext = c.compile();
        assertThat(ztext).containsExactly('Z', '1', 'J', '7', 'b');
    }

    @Test
    public void shouldCreateCoreMatchCodeWithRequiredField() {
        ZscriptCommandNode c = CoreModule.matchCode()
                .setMatchCode(0x3a42)
                .build();
        byte[] ztext = c.compile();
        assertThat(ztext).containsExactly('Z', 'd', 'C', '3', 'a', '4', '2');
    }

    @Test
    public void shouldCreateCoreMatchCodeWithoutRequiredField() {
        assertThatThrownBy(() -> CoreModule.matchCode().build()).isInstanceOf(ZscriptMissingFieldException.class)
                .hasMessage("missingKeys='C'");
    }
}
