package net.zscript.javaclient.core;

import static org.assertj.core.api.Assertions.assertThat;
import static org.junit.jupiter.params.provider.Arguments.of;

import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.Arguments;
import org.junit.jupiter.params.provider.MethodSource;

import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;
import java.util.stream.Stream;

import net.zscript.javaclient.commandPaths.CommandExecutionPath;
import net.zscript.javareceiver.tokenizer.TokenExtendingBuffer;
import net.zscript.javareceiver.tokenizer.Tokenizer;

public class CommandExecutionPathRegenerationTest {

    @ParameterizedTest
    @MethodSource
    public void shouldProduceActionsForLogicalCommandSeries(final String input, final String output) {
        TokenExtendingBuffer bufferCmd    = new TokenExtendingBuffer();
        Tokenizer            tokenizerCmd = new Tokenizer(bufferCmd.getTokenWriter(), 2);
        for (byte b : input.getBytes(StandardCharsets.UTF_8)) {
            tokenizerCmd.accept(b);
        }
        CommandExecutionPath path = CommandExecutionPath.parse(bufferCmd.getTokenReader().getFirstReadToken());

        assertThat(path.toSequence().asString()).isEqualTo(output);
    }

    private static Stream<Arguments> shouldProduceActionsForLogicalCommandSeries() {
        return Stream.of(
                of("\n", ""),
                of("Z\n", "Z"),
                of("Z0000\n", "Z"),
                of("Z1\n", "Z1"),
                of("Zdddd\n", "Zdddd"),
                of("+0102030405\n", "+0102030405"),
                of("\"Hello=0a\"\n", "\"Hello=0a\""),
                of("\"=0a\"+0a\"=0a\"\n", "\"=0a\"+0a\"=0a\""),
                of("&\n", "&"),
                of("A&B\n", "A&B"),
                of("A&&&&B\n", "A&&&&B"),
                of("A|B\n", "A|B"),
                of("A&B&C|D&E&F|G&H&I\n", "A&B&C|D&E&F|G&H&I"),
                of("A(B)C\n", "A(B)C"),
                of("A(B&D)C\n", "A(B&D)C"),
                of("A(B|D)C\n", "A(B|D)C"),
                of("A(B|D)C|E\n", "A(B|D)C|E"),
                of("A&B|D)C|E\n", "A&B|D)C|E"),
                of("A(B|C)|D)X)Y)E|F\n", "A(B|C)|D)X)Y)E|F")
        );
    }
}
