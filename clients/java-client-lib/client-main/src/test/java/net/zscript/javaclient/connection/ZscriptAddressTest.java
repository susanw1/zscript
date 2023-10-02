package net.zscript.javaclient.connection;

import java.util.stream.Stream;

import static net.zscript.javaclient.connection.ZscriptAddress.address;
import static org.assertj.core.api.Assertions.assertThat;

import nl.jqno.equalsverifier.EqualsVerifier;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.Arguments;
import org.junit.jupiter.params.provider.MethodSource;

import net.zscript.javaclient.commandbuilder.ZscriptByteString;
import net.zscript.javaclient.commandbuilder.ZscriptByteString.ZscriptByteStringBuilder;

public class ZscriptAddressTest {

    @Test
    public void equalsContract() {
        EqualsVerifier.forClass(ZscriptAddress.class).verify();
    }

    @ParameterizedTest
    @MethodSource
    public void shouldWrite(ZscriptAddress address, int[] array, String expected) {
        ZscriptByteStringBuilder zbsb = ZscriptByteString.builder();
        address.writeTo(zbsb);
        assertThat(zbsb.asString()).isEqualTo(expected);

        assertThat(address.getAsInts()).containsExactly(array);
        assertThat(address).hasToString(expected);
    }

    static Stream<Arguments> shouldWrite() {
        return Stream.of(
                Arguments.of(address(8, 10, 12), new int[] { 8, 10, 12 }, "@8.a.c"),
                Arguments.of(address(1023, 0, 12), new int[] { 1023, 0, 12 }, "@3ff..c"),
                Arguments.of(address(), new int[0], ""),
                Arguments.of(address(0), new int[] { 0 }, "@"),
                Arguments.of(address(0, 0, 0), new int[] { 0, 0, 0 }, "@.."),
                Arguments.of(address(0, 0, 0, 0, 0), new int[] { 0, 0, 0, 0, 0 }, "@....")
        );
    }
}
