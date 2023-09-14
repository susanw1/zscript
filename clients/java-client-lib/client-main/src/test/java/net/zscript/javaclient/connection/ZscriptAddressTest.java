package net.zscript.javaclient.connection;

import static org.assertj.core.api.Assertions.assertThat;

import org.junit.jupiter.api.Test;

public class ZscriptAddressTest {
    @Test
    public void shouldJustWork() {
        ZscriptAddress address = ZscriptAddress.from(8, 10, 12);
        assertThat(address.getAsInts()).containsExactly(8, 10, 12);
        assertThat(address).hasToString("@8.a.c");
    }

    @Test
    public void shouldHandleZeroAddress() {
        ZscriptAddress address = ZscriptAddress.from(0);
        assertThat(address.getAsInts()).containsExactly(0);
        assertThat(address).hasToString("@");
    }

    @Test
    public void shouldHandleSomeZeroesAddress() {
        ZscriptAddress address = ZscriptAddress.from(0, 0, 0);
        assertThat(address.getAsInts()).containsExactly(0, 0, 0);
        assertThat(address).hasToString("@..");
    }
}
