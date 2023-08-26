package net.zscript.client.modules.base;

import static org.assertj.core.api.Assertions.assertThat;
import static org.assertj.core.api.Assertions.assertThatThrownBy;

import org.junit.jupiter.api.Test;

import net.zscript.javaclient.commandbuilder.ZscriptCommand;
import net.zscript.javaclient.commandbuilder.ZscriptMissingFieldException;

public class CoreModuleTest {
    @Test
    public void shouldCreateCoreCapabilities() {
        ZscriptCommand c = CoreModule.capabilities()
                .build();
        byte[] ztext = c.compile(false);
        assertThat(ztext).containsExactly('Z');
    }

    @Test
    public void shouldCreateCoreActivate() {
        ZscriptCommand c = CoreModule.activate()
                .build();
        byte[] ztext = c.compile(false);
        assertThat(ztext).containsExactly('Z', '2');
    }

    @Test
    public void shouldCreateCoreActivateWithField() {
        ZscriptCommand c = CoreModule.activate()
                .challenge(3)
                .build();
        byte[] ztext = c.compile(false);
        assertThat(ztext).containsExactly('Z', '2', 'K', '3');
    }

    @Test
    public void shouldCreateCoreEcho() {
        ZscriptCommand c = CoreModule.echo()
                .any('J', 123)
                .build();
        byte[] ztext = c.compile(false);
        assertThat(ztext).containsExactly('Z', '1', 'J', '7', 'b');
    }

    @Test
    public void shouldCreateCoreMatchCodeWithRequiredField() {
        ZscriptCommand c = CoreModule.matchCode()
                .matchCode(0x3a42)
                .build();
        byte[] ztext = c.compile(false);
        assertThat(ztext).containsExactly('Z', 'd', 'C', '3', 'a', '4', '2');
    }

    @Test
    public void shouldCreateCoreMatchCodeWithoutRequiredField() {
        assertThatThrownBy(() -> {
            CoreModule.matchCode().build();
        }).isInstanceOf(ZscriptMissingFieldException.class)
                .hasMessage("missingKeys='C'");
    }
}