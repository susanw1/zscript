package net.zscript.javaclient.core;

import static org.assertj.core.api.Assertions.assertThat;
import static org.junit.jupiter.params.provider.Arguments.of;

import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.Arguments;
import org.junit.jupiter.params.provider.MethodSource;

import java.util.stream.Stream;

import net.zscript.javaclient.commandPaths.CommandExecutionPath;
import net.zscript.javaclient.commandbuilder.AbortCommandNode;
import net.zscript.javaclient.commandbuilder.BlankCommandNode;
import net.zscript.javaclient.commandbuilder.CommandSequenceNode;
import net.zscript.javaclient.commandbuilder.FailureCommandNode;
import net.zscript.model.modules.base.CoreModule;

public class CommandPathTreeTest {
    @ParameterizedTest
    @MethodSource
    public void shouldCreateBytesForSingleCommands(final CommandSequenceNode input, final String output) {
        CommandExecutionPath path = CommandExecutionPath.convert(input).getPath();

        assertThat(path.toSequence().asString()).isEqualTo(output);
    }

    private static Stream<Arguments> shouldCreateBytesForSingleCommands() {
        return Stream.of(
                of(new BlankCommandNode(), ""),
                of(new FailureCommandNode(), "Z1S2"),
                of(new AbortCommandNode(), "Z1S13"),
                of(CoreModule.activateBuilder().build(), "Z2")
        );
    }

    @ParameterizedTest
    @MethodSource
    public void shouldCreateBytesForAndSequences(final CommandSequenceNode input, final String output) {
        CommandExecutionPath path = CommandExecutionPath.convert(input).getPath();

        assertThat(path.toSequence().asString()).isEqualTo(output);
    }

    private static Stream<Arguments> shouldCreateBytesForAndSequences() {
        return Stream.of(
                of(CoreModule.activateBuilder().build().andThen(new FailureCommandNode()), "Z2&Z1S2"),
                of(CoreModule.activateBuilder().build().andThen(new AbortCommandNode()), "Z2&Z1S13"),
                of(CoreModule.activateBuilder().build().andThen(new BlankCommandNode()).andThen(new BlankCommandNode()), "Z2&&"),
                of(CoreModule.activateBuilder().setChallenge(0).build().andThen(
                        CoreModule.activateBuilder().setChallenge(1).build().andThen(CoreModule.activateBuilder().setChallenge(2).build())
                ).andThen(CoreModule.activateBuilder().setChallenge(3).build()), "Z2K&Z2K1&Z2K2&Z2K3"),
                of(CoreModule.activateBuilder().setChallenge(0).build().andThen(
                        CoreModule.activateBuilder().setChallenge(1).build().andThen(
                                CoreModule.activateBuilder().setChallenge(2).build().andThen(CoreModule.activateBuilder().setChallenge(4).build())
                        )
                ).andThen(CoreModule.activateBuilder().setChallenge(3).build()), "Z2K&Z2K1&Z2K2&Z2K4&Z2K3")
        );
    }

    @ParameterizedTest
    @MethodSource
    public void shouldCreateBytesForOrSequences(final CommandSequenceNode input, final String output) {
        CommandExecutionPath path = CommandExecutionPath.convert(input).getPath();

        assertThat(path.toSequence().asString()).isEqualTo(output);
    }

    private static Stream<Arguments> shouldCreateBytesForOrSequences() {
        return Stream.of(
                of(CoreModule.activateBuilder().build().onFail(new FailureCommandNode()), "Z2|Z1S2"),
                of(CoreModule.activateBuilder().build().onFail(new AbortCommandNode()), "Z2|Z1S13"),
                of(CoreModule.activateBuilder().build().onFail(new BlankCommandNode()), "Z2|"),
                of(CoreModule.activateBuilder().setChallenge(0).build().onFail(
                        CoreModule.activateBuilder().setChallenge(1).build().onFail(CoreModule.activateBuilder().setChallenge(2).build())
                ).onFail(CoreModule.activateBuilder().setChallenge(3).build()), "Z2K|Z2K1|Z2K2|Z2K3"),
                of(CoreModule.activateBuilder().setChallenge(0).build().onFail(
                        CoreModule.activateBuilder().setChallenge(1).build().onFail(
                                CoreModule.activateBuilder().setChallenge(2).build().onFail(CoreModule.activateBuilder().setChallenge(4).build())
                        )
                ).onFail(CoreModule.activateBuilder().setChallenge(3).build()), "Z2K|Z2K1|Z2K2|Z2K4|Z2K3")
        );
    }

    @ParameterizedTest
    @MethodSource
    public void shouldCreateBytesForMixedSequences(final CommandSequenceNode input, final String output) {
        CommandExecutionPath path = CommandExecutionPath.convert(input).getPath();

        assertThat(path.toSequence().asString()).isEqualTo(output);
    }

    private static Stream<Arguments> shouldCreateBytesForMixedSequences() {
        return Stream.of(
                of(CoreModule.activateBuilder().build().andThen(CoreModule.activateBuilder().build()).onFail(new FailureCommandNode()), "Z2&Z2|Z1S2"),
                of(CoreModule.activateBuilder().build().onFail(CoreModule.activateBuilder().build().andThen(new FailureCommandNode())), "Z2|Z2&Z1S2"),
                of(CoreModule.activateBuilder().build().andThen(CoreModule.activateBuilder().build().onFail(new FailureCommandNode())), "Z2(Z2|Z1S2"),
                of(CoreModule.activateBuilder().build().onFail(new FailureCommandNode()).andThen(CoreModule.activateBuilder().build()), "Z2|Z1S2)Z2"),
                of(CoreModule.activateBuilder().build().andThen(CoreModule.activateBuilder().build().dropFailureCondition()).andThen(new FailureCommandNode()), "Z2(Z2|)Z1S2"),
                of(CoreModule.activateBuilder().build().andThen(CoreModule.activateBuilder().build().onFail(new FailureCommandNode())).andThen(new AbortCommandNode()),
                        "Z2(Z2|Z1S2)Z1S13"),
                of(CoreModule.activateBuilder().build().andThen(
                        CoreModule.activateBuilder().build().onFail(new FailureCommandNode())
                ).andThen(
                        CoreModule.activateBuilder().build().onFail(new FailureCommandNode())
                ).andThen(new AbortCommandNode()), "Z2(Z2|Z1S2)(Z2|Z1S2)Z1S13"),
                of(CoreModule.activateBuilder().build().andThen(
                        CoreModule.activateBuilder().build().onFail(new FailureCommandNode()).andThen(CoreModule.activateBuilder().build()).onFail(new FailureCommandNode())
                ).andThen(new AbortCommandNode()), "Z2((Z2|Z1S2)Z2|Z1S2)Z1S13"),
                of(CoreModule.activateBuilder().setChallenge(1).build().andThen(
                        CoreModule.activateBuilder().setChallenge(2).build().onFail(
                                CoreModule.activateBuilder().setChallenge(3).build().andThen(
                                        CoreModule.activateBuilder().setChallenge(4).build().onFail(new FailureCommandNode())
                                )
                        )
                ).andThen(new AbortCommandNode()), "Z2K1(Z2K2|Z2K3(Z2K4|Z1S2))Z1S13")
        );
    }

}
