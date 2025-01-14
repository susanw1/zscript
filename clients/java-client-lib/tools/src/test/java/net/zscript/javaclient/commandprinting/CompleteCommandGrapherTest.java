package net.zscript.javaclient.commandprinting;

import java.util.stream.Stream;

import static java.util.stream.Collectors.joining;
import static net.zscript.util.ByteString.byteStringUtf8;
import static org.assertj.core.api.Assertions.assertThat;
import static org.junit.jupiter.params.provider.Arguments.of;

import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.Arguments;
import org.junit.jupiter.params.provider.MethodSource;

import net.zscript.ascii.AnsiCharacterStylePrinter;
import net.zscript.javaclient.commandpaths.CommandExecutionPath;
import net.zscript.javaclient.tokens.ExtendingTokenBuffer;

public class CompleteCommandGrapherTest {
    CommandGraph.GraphPrintSettings basicSettings = new CommandGraph.GraphPrintSettings(new StandardCommandGrapher.CommandPrintSettings("  ", VerbositySetting.NAME), true, 2,
            60);

    @ParameterizedTest
    @MethodSource
    public void shouldProduceGoodGraphs(final String input, final String output) {
        final ExtendingTokenBuffer bufferCmd = ExtendingTokenBuffer.tokenize(byteStringUtf8(input), false);
        final CommandExecutionPath path      = CommandExecutionPath.parse(bufferCmd.getTokenReader().getFirstReadToken());

        String ANSI_ANYTHING = "(\u001B[\\[\\d;]*m)*";
        assertThat(new StandardCommandGrapher().graph(path, null, basicSettings)
                .generateString(new AnsiCharacterStylePrinter())
                .replaceAll(ANSI_ANYTHING, "")
                .lines()
                .map(s -> s.stripTrailing() + "\n").collect(joining()))
                .isEqualTo(output);
    }

    private static Stream<Arguments> shouldProduceGoodGraphs() {
        return Stream.of(
                of("A\n", "O" + " - No command field - Z expected: A\n"
                        + "  ---------------------------------------------------------\n"
                        + "\n"
                ),
                of("A&B\n", "O - No command field - Z expected: A\n"
                        + "| ---------------------------------------------------------\n"
                        + "|\n"
                        + "O - No command field - Z expected: B\n"
                        + "  ---------------------------------------------------------\n"
                        + "\n"
                ),
                of("A|B\n", "O    - No command field - Z expected: A\n"
                        + " \\   ------------------------------------------------------\n"
                        + "  \\\n"
                        + "   O - No command field - Z expected: B\n"
                        + "     ------------------------------------------------------\n"
                        + "\n"
                ),
                of("A&B|C\n", "O    - No command field - Z expected: A\n"
                        + "|\\   ------------------------------------------------------\n"
                        + "| \\\n"
                        + "O  | - No command field - Z expected: B\n"
                        + " \\ | ------------------------------------------------------\n"
                        + "  \\|\n"
                        + "   O - No command field - Z expected: C\n"
                        + "     ------------------------------------------------------\n"
                        + "\n"
                ),
                of("X(A&B|C)Y\n", "O    - No command field - Z expected: X\n"
                        + "|    ------------------------------------------------------\n"
                        + "|\n"
                        + "O    - No command field - Z expected: A\n"
                        + "|\\   ------------------------------------------------------\n"
                        + "| \\\n"
                        + "O  | - No command field - Z expected: B\n"
                        + "|\\ | ------------------------------------------------------\n"
                        + "| \\|\n"
                        + "|  O - No command field - Z expected: C\n"
                        + "| /  ------------------------------------------------------\n"
                        + "|/\n"
                        + "O    - No command field - Z expected: Y\n"
                        + "     ------------------------------------------------------\n"
                        + "\n"
                ),
                of("X(A|B&C)Y\n", "O    - No command field - Z expected: X\n"
                        + "|    ------------------------------------------------------\n"
                        + "|\n"
                        + "O    - No command field - Z expected: A\n"
                        + "|\\   ------------------------------------------------------\n"
                        + "| \\\n"
                        + "|  O - No command field - Z expected: B\n"
                        + "|  | ------------------------------------------------------\n"
                        + "|  |\n"
                        + "|  O - No command field - Z expected: C\n"
                        + "| /  ------------------------------------------------------\n"
                        + "|/\n"
                        + "O    - No command field - Z expected: Y\n"
                        + "     ------------------------------------------------------\n"
                        + "\n"
                ),
                of("X(A|B&C)Y|Z\n", "O       - No command field - Z expected: X\n"
                        + "|\\___   ---------------------------------------------------\n"
                        + "|    \\\n"
                        + "O     | - No command field - Z expected: A\n"
                        + "|\\    | ---------------------------------------------------\n"
                        + "| \\   |\n"
                        + "|  O  | - No command field - Z expected: B\n"
                        + "|  |\\ | ---------------------------------------------------\n"
                        + "|  | \\|\n"
                        + "|  O  | - No command field - Z expected: C\n"
                        + "| / \\ | ---------------------------------------------------\n"
                        + "|/   \\|\n"
                        + "O     | - No command field - Z expected: Y\n"
                        + " \\___ | ---------------------------------------------------\n"
                        + "     \\|\n"
                        + "      O - Core Capabilities: Z\n"
                        + "        ---------------------------------------------------\n"
                        + "\n"
                ),
                of("X(A|B&C)Y|Z)W\n", "O       - No command field - Z expected: X\n"
                        + "|\\___   ---------------------------------------------------\n"
                        + "|    \\\n"
                        + "O     | - No command field - Z expected: A\n"
                        + "|\\    | ---------------------------------------------------\n"
                        + "| \\   |\n"
                        + "|  O  | - No command field - Z expected: B\n"
                        + "|  |\\ | ---------------------------------------------------\n"
                        + "|  | \\|\n"
                        + "|  O  | - No command field - Z expected: C\n"
                        + "| / \\ | ---------------------------------------------------\n"
                        + "|/   \\|\n"
                        + "O     | - No command field - Z expected: Y\n"
                        + "|\\___ | ---------------------------------------------------\n"
                        + "|    \\|\n"
                        + "|     O - Core Capabilities: Z\n"
                        + "| ___/  ---------------------------------------------------\n"
                        + "|/\n"
                        + "O       - No command field - Z expected: W\n"
                        + "        ---------------------------------------------------\n"
                        + "\n"
                )
        );

    }
}
