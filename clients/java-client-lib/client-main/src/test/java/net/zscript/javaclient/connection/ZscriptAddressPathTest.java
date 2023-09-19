package net.zscript.javaclient.connection;

import static net.zscript.javaclient.connection.ZscriptAddress.address;
import static net.zscript.javaclient.connection.ZscriptAddressPath.path;
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

public class ZscriptAddressPathTest {
    @Test
    public void equalsContract() {
        EqualsVerifier.forClass(ZscriptAddressPath.class).verify();
    }

    @ParameterizedTest
    @MethodSource
    public void shouldWrite(ZscriptAddressPath addressPath, String expected) throws IOException {
        StringWriter       writer             = new StringWriter();
        WriterOutputStream writerOutputStream = WriterOutputStream.builder().setWriter(writer).get();

        addressPath.writeTo(writerOutputStream).flush();
        assertThat(writer).hasToString(expected);
    }

    static Stream<Arguments> shouldWrite() {
        return Stream.of(
                Arguments.of(path(address(8, 10, 12), address(15, 16, 17)), "@8.a.c@f.10.11"),
                Arguments.of(path(address(1023, 0, 12)), "@3ff..c"),
                Arguments.of(path(address(), address()), ""),
                Arguments.of(path(address(0), address(0)), "@@"),
                Arguments.of(path(address(0, 0, 0), address(0, 0)), "@..@.")
        );
    }
}
