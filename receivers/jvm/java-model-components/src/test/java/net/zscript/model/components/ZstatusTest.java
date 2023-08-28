package net.zscript.model.components;

import static org.assertj.core.api.Assertions.assertThat;

import org.junit.jupiter.api.Test;

public class ZstatusTest {
    @Test
    public void shouldCheckZchars() {
        assertThat(ZscriptStatus.SUCCESS).isEqualTo((byte) 0);
        assertThat(ZscriptStatus.COMMAND_FAIL).isEqualTo((byte) 1);
        assertThat(ZscriptStatus.BUFFER_OVR_ERROR).isEqualTo((byte) 0x10);
    }

    @Test
    public void shouldIdentifySuccess() {
        assertThat(ZscriptStatus.isSuccess(ZscriptStatus.SUCCESS)).isTrue();
        assertThat(ZscriptStatus.isSuccess(ZscriptStatus.COMMAND_FAIL)).isFalse();
        assertThat(ZscriptStatus.isSuccess(ZscriptStatus.BUFFER_OVR_ERROR)).isFalse();
    }

    @Test
    public void shouldIdentifyFailure() {
        assertThat(ZscriptStatus.isFailure(ZscriptStatus.SUCCESS)).isFalse();
        assertThat(ZscriptStatus.isFailure(ZscriptStatus.COMMAND_FAIL)).isTrue();
        assertThat(ZscriptStatus.isFailure(ZscriptStatus.BUFFER_OVR_ERROR)).isFalse();
    }

    @Test
    public void shouldIdentifyError() {
        assertThat(ZscriptStatus.isError(ZscriptStatus.SUCCESS)).isFalse();
        assertThat(ZscriptStatus.isError(ZscriptStatus.COMMAND_FAIL)).isFalse();
        assertThat(ZscriptStatus.isError(ZscriptStatus.BUFFER_OVR_ERROR)).isTrue();
    }
}
