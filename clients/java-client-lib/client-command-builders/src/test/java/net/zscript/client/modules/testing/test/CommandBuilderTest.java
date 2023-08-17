package net.zscript.client.modules.testing.test;

import static org.assertj.core.api.Assertions.assertThat;

import org.junit.jupiter.api.Test;

import net.zscript.client.modules.test.testing.TestingModule;
import net.zscript.client.modules.test.testing.TestingModule.CapabilitiesTesterCommandBuilder.VersionType;

public class CommandBuilderTest {
    @Test
    void should() {
        byte[] s = TestingModule.capabilitiesTester()
                .versionType(VersionType.PlatformFirmware)
                .build()
                .compile(true);
        assertThat(s).containsExactly('Z', 'V', '2');
    }
}
