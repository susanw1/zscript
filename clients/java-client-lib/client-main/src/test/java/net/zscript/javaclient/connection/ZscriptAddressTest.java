package net.zscript.javaclient.connection;

import static net.zscript.javaclient.connection.ZscriptAddress.address;
import static org.assertj.core.api.Assertions.assertThat;

import nl.jqno.equalsverifier.EqualsVerifier;
import org.apache.commons.io.output.WriterOutputStream;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.Arguments;
import org.junit.jupiter.params.provider.MethodSource;

import java.io.IOException;
import java.io.StringWriter;
import java.util.stream.Stream;

public class ZscriptAddressTest {

    @Test
    public void equalsContract() {
        EqualsVerifier.forClass(ZscriptAddress.class).verify();
    }

    @ParameterizedTest
    @MethodSource
    public void shouldWrite(ZscriptAddress address, int[] array, String expected) throws IOException {
        StringWriter       writer             = new StringWriter();
        WriterOutputStream writerOutputStream = WriterOutputStream.builder().setWriter(writer).get();

        address.writeTo(writerOutputStream).flush();
        assertThat(writer).hasToString(expected);

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
