package net.zscript.javaclient.commandPrinting;

import java.nio.charset.StandardCharsets;
import java.util.stream.Collectors;
import java.util.stream.Stream;

import static org.assertj.core.api.Assertions.assertThat;
import static org.junit.jupiter.params.provider.Arguments.of;

import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.Arguments;
import org.junit.jupiter.params.provider.MethodSource;

import net.zscript.ascii.AnsiCharacterStylePrinter;
import net.zscript.javaclient.commandPaths.CommandExecutionPath;
import net.zscript.javareceiver.tokenizer.TokenExtendingBuffer;
import net.zscript.javareceiver.tokenizer.Tokenizer;

public class CompleteCommandGrapherTest {
    CommandGraph.GraphPrintSettings basicSettings = new CommandGraph.GraphPrintSettings(new StandardCommandGrapher.CommandPrintSettings("  ", VerbositySetting.NAME), true, 2,
            60);

    @ParameterizedTest
    @MethodSource
    public void shouldProduceGoodGraphs(final String input, final String output) {
        TokenExtendingBuffer bufferCmd    = new TokenExtendingBuffer();
        Tokenizer            tokenizerCmd = new Tokenizer(bufferCmd.getTokenWriter(), 2);
        for (byte b : input.getBytes(StandardCharsets.UTF_8)) {
            tokenizerCmd.accept(b);
        }
        CommandExecutionPath path = CommandExecutionPath.parse(bufferCmd.getTokenReader().getFirstReadToken());

        String ANSI_ANYTHING = "(\u001B[\\[\\d;]*m)*";
        assertThat(new StandardCommandGrapher().graph(path, null, basicSettings).generateString(new AnsiCharacterStylePrinter()).replaceAll(ANSI_ANYTHING, "").lines()
                .map(s -> s.stripTrailing() + "\n").collect(
                        Collectors.joining())).
                isEqualTo(output);
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
