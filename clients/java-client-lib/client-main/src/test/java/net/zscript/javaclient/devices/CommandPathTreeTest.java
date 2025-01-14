package net.zscript.javaclient.devices;

import java.util.stream.Stream;

import static org.assertj.core.api.Assertions.assertThat;
import static org.junit.jupiter.params.provider.Arguments.of;

import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.Arguments;
import org.junit.jupiter.params.provider.MethodSource;

import net.zscript.javaclient.commandbuilder.commandnodes.CommandSequenceNode;
import net.zscript.javaclient.commandbuilder.defaultcommands.AbortCommandNode;
import net.zscript.javaclient.commandbuilder.defaultcommands.BlankCommandNode;
import net.zscript.javaclient.commandbuilder.defaultcommands.FailureCommandNode;
import net.zscript.javaclient.commandpaths.CommandExecutionPath;
import net.zscript.model.ZscriptModel;
import net.zscript.model.modules.base.CoreModule;

public class CommandPathTreeTest {
    @ParameterizedTest
    @MethodSource
    public void shouldCreateBytesForSingleCommands(final CommandSequenceNode input, final String output) {
        Device d = new Device(ZscriptModel.standardModel(), null);
        CommandExecutionPath path = d.convert(input, r -> {
        }).getPath();

        assertThat(path.toByteString().asString()).isEqualTo(output);
    }

    private static Stream<Arguments> shouldCreateBytesForSingleCommands() {
        return Stream.of(
                of(new BlankCommandNode(), ""),
                of(new FailureCommandNode(), "Z1S2"),
                of(new AbortCommandNode(), "Z1S13"),
                of(makeActivateCommand(), "Z2")
        );
    }

    @ParameterizedTest
    @MethodSource
    public void shouldCreateBytesForAndSequences(final CommandSequenceNode input, final String output) {
        Device d = new Device(ZscriptModel.standardModel(), null);
        CommandExecutionPath path = d.convert(input, r -> {
        }).getPath();

        assertThat(path.toByteString().asString()).isEqualTo(output);
    }

    private static Stream<Arguments> shouldCreateBytesForAndSequences() {
        return Stream.of(
                of(makeActivateCommand().andThen(new FailureCommandNode()), "Z2&Z1S2"),
                of(makeActivateCommand().andThen(new AbortCommandNode()), "Z2&Z1S13"),
                of(makeActivateCommand().andThen(new BlankCommandNode()).andThen(new BlankCommandNode()), "Z2&&"),
                of(makeActivateCommand(0).andThen(makeActivateCommand(1).andThen(makeActivateCommand(2))).andThen(makeActivateCommand(3)), "Z2K&Z2K1&Z2K2&Z2K3"),
                of(makeActivateCommand(0).andThen(makeActivateCommand(1).andThen(makeActivateCommand(2)
                        .andThen(makeActivateCommand(4)))).andThen(makeActivateCommand(3)), "Z2K&Z2K1&Z2K2&Z2K4&Z2K3")
        );
    }

    @ParameterizedTest
    @MethodSource
    public void shouldCreateBytesForOrSequences(final CommandSequenceNode input, final String output) {
        Device d = new Device(ZscriptModel.standardModel(), null);
        CommandExecutionPath path = d.convert(input, r -> {
        }).getPath();

        assertThat(path.toByteString().asString()).isEqualTo(output);
    }

    private static Stream<Arguments> shouldCreateBytesForOrSequences() {
        return Stream.of(
                of(makeActivateCommand().onFail(new FailureCommandNode()), "Z2|Z1S2"),
                of(makeActivateCommand().onFail(new AbortCommandNode()), "Z2|Z1S13"),
                of(makeActivateCommand().onFail(new BlankCommandNode()), "Z2|"),
                of(makeActivateCommand(0).onFail(makeActivateCommand(1).onFail(makeActivateCommand(2))).onFail(makeActivateCommand(3)), "Z2K|Z2K1|Z2K2|Z2K3"),
                of(makeActivateCommand(0).onFail(makeActivateCommand(1).onFail(makeActivateCommand(2).onFail(makeActivateCommand(4)))
                ).onFail(makeActivateCommand(3)), "Z2K|Z2K1|Z2K2|Z2K4|Z2K3")
        );
    }

    @ParameterizedTest
    @MethodSource
    public void shouldCreateBytesForMixedSequences(final CommandSequenceNode input, final String output) {
        Device d = new Device(ZscriptModel.standardModel(), null);
        CommandExecutionPath path = d.convert(input, r -> {
        }).getPath();

        assertThat(path.toByteString().asString()).isEqualTo(output);
    }

    private static Stream<Arguments> shouldCreateBytesForMixedSequences() {
        return Stream.of(
                of(makeActivateCommand()
                        .andThen(makeActivateCommand())
                        .onFail(new FailureCommandNode()), "Z2&Z2|Z1S2"),
                of(makeActivateCommand().onFail(makeActivateCommand().andThen(new FailureCommandNode())), "Z2|Z2&Z1S2"),
                of(makeActivateCommand().andThen(makeActivateCommand().onFail(new FailureCommandNode())), "Z2(Z2|Z1S2"),
                of(makeActivateCommand().onFail(new FailureCommandNode()).andThen(makeActivateCommand()), "Z2|Z1S2)Z2"),
                of(makeActivateCommand().andThen(makeActivateCommand().dropFailureCondition()).andThen(new FailureCommandNode()), "Z2(Z2|)Z1S2"),
                of(makeActivateCommand().andThen(makeActivateCommand().onFail(new FailureCommandNode())).andThen(new AbortCommandNode()), "Z2(Z2|Z1S2)Z1S13"),
                of(makeActivateCommand().andThen(makeActivateCommand().onFail(new FailureCommandNode())).andThen(makeActivateCommand().onFail(new FailureCommandNode()))
                        .andThen(new AbortCommandNode()), "Z2(Z2|Z1S2)(Z2|Z1S2)Z1S13"),
                of(makeActivateCommand().andThen(makeActivateCommand().onFail(new FailureCommandNode()).andThen(makeActivateCommand()).onFail(new FailureCommandNode()))
                        .andThen(new AbortCommandNode()), "Z2((Z2|Z1S2)Z2|Z1S2)Z1S13"),
                of(makeActivateCommand(1).andThen(makeActivateCommand(2).onFail(makeActivateCommand(3).andThen(makeActivateCommand(4).onFail(new FailureCommandNode()))))
                        .andThen(new AbortCommandNode()), "Z2K1(Z2K2|Z2K3(Z2K4|Z1S2))Z1S13")
        );
    }

    private static CoreModule.ActivateCommand makeActivateCommand(int challenge) {
        return CoreModule.activateBuilder().setChallenge(challenge).build();
    }

    private static CoreModule.ActivateCommand makeActivateCommand() {
        return CoreModule.activateBuilder().build();
    }
}
