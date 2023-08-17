package net.zscript.client.modules.base;

import static org.assertj.core.api.Assertions.assertThat;
import static org.assertj.core.api.Assertions.assertThatThrownBy;

import org.junit.jupiter.api.Test;

import net.zscript.javaclient.commandbuilder.ZscriptCommand;

public class CoreModuleTest {
    @Test
    public void shouldCreateCoreCapabilities() {
        ZscriptCommand c     = CoreModule.capabilities()
                .build();
        byte[]         ztext = c.compile(false);
        assertThat(ztext).containsExactly('Z');
    }

    @Test
    public void shouldCreateCoreActivate() {
        ZscriptCommand c     = CoreModule.activate()
                .build();
        byte[]         ztext = c.compile(false);
        assertThat(ztext).containsExactly('Z', '2');
    }

    @Test
    public void shouldCreateCoreActivateWithField() {
        ZscriptCommand c     = CoreModule.activate()
                .challenge(3)
                .build();
        byte[]         ztext = c.compile(false);
        assertThat(ztext).containsExactly('Z', '2', 'K', '3');
    }

    @Test
    public void shouldCreateCoreMatchCodeWithRequiredField() {
        ZscriptCommand c     = CoreModule.matchCode()
                .matchCode(0x3a42)
                .build();
        byte[]         ztext = c.compile(false);
        assertThat(ztext).containsExactly('Z', 'd', 'C', '3', 'a', '4', '2');
    }

    @Test
    public void shouldCreateCoreMatchCodeWithoutRequiredField() {
        assertThatThrownBy(() -> {
            CoreModule.matchCode().build();
        }).isInstanceOf(IllegalStateException.class)
                .hasMessage("Required field not set: C");
    }

}
