package net.zscript.javaclient.commandbuilderapi.nodes;

import javax.annotation.Nullable;
import java.util.stream.Stream;

import static org.assertj.core.api.Assertions.assertThat;
import static org.junit.jupiter.params.provider.Arguments.of;

import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.Arguments;
import org.junit.jupiter.params.provider.MethodSource;

import net.zscript.javaclient.commandbuilderapi.DemoActivateCommand;
import net.zscript.javaclient.commandbuilderapi.defaultcommands.AbortCommandNode;
import net.zscript.javaclient.commandbuilderapi.defaultcommands.BlankCommandNode;
import net.zscript.javaclient.commandbuilderapi.defaultcommands.FailureCommandNode;

public class CommandSequenceNodeTest {
    @ParameterizedTest
    @MethodSource
    public void shouldCreateBytesForSingleCommands(final CommandSequenceNode input, final String rawOutput, @Nullable String optOutput) {
        assertThat(input.asStringUtf8()).isEqualTo(rawOutput);
        assertThat(input.optimize().asStringUtf8()).isEqualTo(optOutput != null ? optOutput : rawOutput);
    }

    private static Stream<Arguments> shouldCreateBytesForSingleCommands() {
        return Stream.of(
                of(new BlankCommandNode(), "", null),
                of(new FailureCommandNode(), "Z1S2", null),
                of(new AbortCommandNode(), "Z1S13", null),
                of(makeActivateCommand(), "Z2", null)
        );
    }

    @ParameterizedTest
    @MethodSource
    public void shouldCreateBytesForAndSequences(final CommandSequenceNode input, final String rawOutput, @Nullable String optOutput) {
        assertThat(input.asStringUtf8()).isEqualTo(rawOutput);
        assertThat(input.optimize().asStringUtf8()).isEqualTo(optOutput != null ? optOutput : rawOutput);
    }

    private static Stream<Arguments> shouldCreateBytesForAndSequences() {
        return Stream.of(
                of(makeActivateCommand().andThen(new FailureCommandNode()), "Z2&Z1S2", null),
                of(makeActivateCommand().andThen(new BlankCommandNode()), "Z2&", "Z2"),
                of(new BlankCommandNode().andThen(new BlankCommandNode()), "", null),
                of((makeActivateCommand().andThen(makeActivateCommand()))
                        .andThen(makeActivateCommand().andThen(new BlankCommandNode())), "Z2&Z2&Z2&", "Z2&Z2&Z2"),
                of(makeActivateCommand().andThen(new AbortCommandNode()), "Z2&Z1S13", null),
                of(makeActivateCommand()
                        .andThen(new BlankCommandNode())
                        .andThen(new BlankCommandNode()), "Z2&&", "Z2"),
                of(makeActivateCommand(0)
                        .andThen(makeActivateCommand(1).andThen(makeActivateCommand(2)))
                        .andThen(makeActivateCommand(3)), "Z2K&Z2K1&Z2K2&Z2K3", null),
                of(makeActivateCommand(0)
                        .andThen(makeActivateCommand(1).andThen(makeActivateCommand(2).andThen(makeActivateCommand(4))))
                        .andThen(makeActivateCommand(3)), "Z2K&Z2K1&Z2K2&Z2K4&Z2K3", null)
        );
    }

    @ParameterizedTest
    @MethodSource
    public void shouldCreateBytesForOrSequences(final CommandSequenceNode input, final String rawOutput, @Nullable String optOutput) {
        assertThat(input.asStringUtf8()).isEqualTo(rawOutput);
        assertThat(input.optimize().asStringUtf8()).isEqualTo(optOutput != null ? optOutput : rawOutput);
    }

    private static Stream<Arguments> shouldCreateBytesForOrSequences() {
        return Stream.of(
                of(makeActivateCommand().onFail(new FailureCommandNode()), "Z2|Z1S2", null),
                of(makeActivateCommand().onFail(new AbortCommandNode()), "Z2|Z1S13", null),
                of(makeActivateCommand().onFail(new BlankCommandNode()), "Z2|", null),
                of(new BlankCommandNode().onFail(new FailureCommandNode()), "", null),
                of((makeActivateCommand().onFail(makeActivateCommand()))
                        .onFail(makeActivateCommand().onFail(new BlankCommandNode())), "Z2|Z2|Z2|", null),
                of(makeActivateCommand(0)
                        .onFail(makeActivateCommand(1).onFail(makeActivateCommand(2)))
                        .onFail(makeActivateCommand(3)), "Z2K|(Z2K1|Z2K2)|Z2K3", "Z2K|Z2K1|Z2K2|Z2K3"),
                of(makeActivateCommand(0)
                        .onFail(makeActivateCommand(1).onFail(makeActivateCommand(2).onFail(makeActivateCommand(4))))
                        .onFail(makeActivateCommand(3)), "Z2K|(Z2K1|(Z2K2|Z2K4))|Z2K3", "Z2K|Z2K1|Z2K2|Z2K4|Z2K3")
        );
    }

    @ParameterizedTest
    @MethodSource
    public void shouldCreateBytesForMixedSequences(final CommandSequenceNode input, final String rawOutput, @Nullable String optOutput) {
        assertThat(input.asStringUtf8()).isEqualTo(rawOutput);
        assertThat(input.optimize().asStringUtf8()).isEqualTo(optOutput != null ? optOutput : rawOutput);
    }

    private static Stream<Arguments> shouldCreateBytesForMixedSequences() {
        return Stream.of(
                of(makeActivateCommand()
                        .andThen(makeActivateCommand())
                        .onFail(new FailureCommandNode()), "Z2&Z2|Z1S2", null),
                of(makeActivateCommand()
                        .onFail(makeActivateCommand().andThen(new FailureCommandNode())), "Z2|Z2&Z1S2", null),
                of(makeActivateCommand()
                        .andThen(makeActivateCommand().onFail(new FailureCommandNode())), "Z2&(Z2|Z1S2)", null),
                of(makeActivateCommand().onFail(new FailureCommandNode()).andThen(makeActivateCommand()), "(Z2|Z1S2)&Z2", null),
                of(makeActivateCommand()
                        .andThen(makeActivateCommand().dropFailureCondition())
                        .andThen(new FailureCommandNode()), "Z2&(Z2|)&Z1S2", null),
                of(makeActivateCommand()
                        .andThen(makeActivateCommand()
                                .onFail(new FailureCommandNode()))
                        .andThen(new AbortCommandNode()), "Z2&(Z2|Z1S2)&Z1S13", null),
                of(makeActivateCommand()
                        .andThen(makeActivateCommand().onFail(new FailureCommandNode()))
                        .andThen(makeActivateCommand().onFail(new FailureCommandNode()))
                        .andThen(new AbortCommandNode()), "Z2&(Z2|Z1S2)&(Z2|Z1S2)&Z1S13", null),
                of(makeActivateCommand()
                        .andThen(makeActivateCommand()
                                .onFail(new FailureCommandNode())
                                .andThen(makeActivateCommand())
                                .onFail(new FailureCommandNode()))
                        .andThen(new AbortCommandNode()), "Z2&((Z2|Z1S2)&Z2|Z1S2)&Z1S13", null),
                of(makeActivateCommand(1)
                        .andThen(makeActivateCommand(2).onFail(makeActivateCommand(3).andThen(makeActivateCommand(4).onFail(new FailureCommandNode()))))
                        .andThen(new AbortCommandNode()), "Z2K1&(Z2K2|Z2K3&(Z2K4|Z1S2))&Z1S13", null)
        );
    }

    private static DemoActivateCommand makeActivateCommand(int challenge) {
        return new DemoActivateCommand.DemoActivateCommandBuilder().setChallenge(challenge).build();
    }

    private static DemoActivateCommand makeActivateCommand() {
        return new DemoActivateCommand.DemoActivateCommandBuilder().build();
    }
}
