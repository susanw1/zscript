package net.zscript.javaclient.core;

import static org.assertj.core.api.Assertions.assertThat;
import static org.junit.jupiter.params.provider.Arguments.of;

import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.Arguments;
import org.junit.jupiter.params.provider.MethodSource;

import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;
import java.util.List;
import java.util.stream.Stream;

import net.zscript.javareceiver.semanticParser.SemanticAction;
import net.zscript.javareceiver.tokenizer.TokenExtendingBuffer;
import net.zscript.javareceiver.tokenizer.Tokenizer;

public class CompleteCommandGrapherTest {
    public static final String ANSI_RESET  = "\u001B[0m";
    public static final String ANSI_BOLD   = "\u001B[1m";
    public static final String ANSI_BLACK  = "\u001B[30m";
    public static final String ANSI_RED    = "\u001B[31m";
    public static final String ANSI_GREEN  = "\u001B[32m";
    public static final String ANSI_YELLOW = "\u001B[33m";
    public static final String ANSI_BLUE   = "\u001B[34m";
    public static final String ANSI_PURPLE = "\u001B[35m";
    public static final String ANSI_CYAN   = "\u001B[36m";
    public static final String ANSI_WHITE  = "\u001B[37m";

    @ParameterizedTest
    @MethodSource
    public void shouldProduceGoodGraphs(final String input, final String output) {
        TokenExtendingBuffer bufferCmd    = new TokenExtendingBuffer();
        Tokenizer            tokenizerCmd = new Tokenizer(bufferCmd.getTokenWriter(), 2);
        for (byte b : input.getBytes(StandardCharsets.UTF_8)) {
            tokenizerCmd.accept(b);
        }
        CommandExecutionPath path = CommandExecutionPath.parse(bufferCmd.getTokenReader().getFirstReadToken());

        String ANSI_ANYTHING = "(\u001B[\\[\\dm]*)*";
        assertThat(path.graphPrint(new StandardCommandGrapher(), false).replaceAll(ANSI_ANYTHING, "")).isEqualTo(output);
    }

    private static Stream<Arguments> shouldProduceGoodGraphs() {
        return Stream.of(
                of("A\n", "O" + "   Command description\n"
                        + " \n"
                        + " \n"
                ),
                of("A&B\n", "O   Command description\n"
                        + "|\n"
                        + "|\n"
                        + "O   Command description\n"
                        + " \n"
                        + " \n"
                ),
                of("A|B\n", "O      Command description\n"
                        + " \\  \n"
                        + "  \\ \n"
                        + "   O   Command description\n"
                        + "    \n"
                        + "    \n"
                ),
                of("A&B|C\n", "O      Command description\n"
                        + "|\\  \n"
                        + "| \\ \n"
                        + "O  |   Command description\n"
                        + " \\ |\n"
                        + "  \\|\n"
                        + "   O   Command description\n"
                        + "    \n"
                        + "    \n"
                ),
                of("X(A&B|C)Y\n", "O      Command description\n"
                        + "|   \n"
                        + "|   \n"
                        + "O      Command description\n"
                        + "|\\  \n"
                        + "| \\ \n"
                        + "O  |   Command description\n"
                        + "|\\ |\n"
                        + "| \\|\n"
                        + "|  O   Command description\n"
                        + "| / \n"
                        + "|/  \n"
                        + "O      Command description\n"
                        + "    \n"
                        + "    \n"
                ),
                of("X(A|B&C)Y\n", "O      Command description\n"
                        + "|   \n"
                        + "|   \n"
                        + "O      Command description\n"
                        + "|\\  \n"
                        + "| \\ \n"
                        + "|  O   Command description\n"
                        + "|  |\n"
                        + "|  |\n"
                        + "|  O   Command description\n"
                        + "| / \n"
                        + "|/  \n"
                        + "O      Command description\n"
                        + "    \n"
                        + "    \n"
                ),
                of("X(A|B&C)Y|Z\n", "O         Command description\n"
                        + "|\\___  \n"
                        + "|    \\ \n"
                        + "O     |   Command description\n"
                        + "|\\    |\n"
                        + "| \\   |\n"
                        + "|  O  |   Command description\n"
                        + "|  |\\ |\n"
                        + "|  | \\|\n"
                        + "|  O  |   Command description\n"
                        + "| / \\ |\n"
                        + "|/   \\|\n"
                        + "O     |   Command description\n"
                        + " \\___ |\n"
                        + "     \\|\n"
                        + "      O   Command description\n"
                        + "       \n"
                        + "       \n"
                ),
                of("X(A|B&C)Y|Z)W\n", "O         Command description\n"
                        + "|\\___  \n"
                        + "|    \\ \n"
                        + "O     |   Command description\n"
                        + "|\\    |\n"
                        + "| \\   |\n"
                        + "|  O  |   Command description\n"
                        + "|  |\\ |\n"
                        + "|  | \\|\n"
                        + "|  O  |   Command description\n"
                        + "| / \\ |\n"
                        + "|/   \\|\n"
                        + "O     |   Command description\n"
                        + "|\\___ |\n"
                        + "|    \\|\n"
                        + "|     O   Command description\n"
                        + "| ___/ \n"
                        + "|/     \n"
                        + "O         Command description\n"
                        + "       \n"
                        + "       \n"
                )
        );

    }
}
