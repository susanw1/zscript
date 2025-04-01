package net.zscript.javaclient.commandbuilderapi.defaultcommands;

import java.util.List;
import java.util.stream.Stream;

import static java.util.Collections.emptyList;
import static net.zscript.javaclient.commandbuilderapi.nodes.CommandSequenceNode.ifFails;
import static net.zscript.javaclient.commandbuilderapi.nodes.CommandSequenceNode.ifSucceeds;
import static net.zscript.javaclient.commandpaths.FieldElement.fieldOf;
import static org.assertj.core.api.Assertions.assertThat;
import static org.junit.jupiter.params.provider.Arguments.of;

import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.Arguments;
import org.junit.jupiter.params.provider.MethodSource;

import net.zscript.javaclient.commandbuilderapi.nodes.CommandSequenceNode;
import net.zscript.javaclient.commandbuilderapi.nodes.ZscriptCommandNode;
import net.zscript.javaclient.commandpaths.ZscriptFieldSet;

class ConditionalNodeTest {
    static final ZscriptCommandNode<DefaultResponse> cmd1 = new GenericCommandBuilder().setField('A', 0).build();
    static final ZscriptCommandNode<DefaultResponse> cmd2 = new GenericCommandBuilder().setField('B', 0).build();
    static final ZscriptCommandNode<DefaultResponse> cmd3 = new GenericCommandBuilder().setField('C', 0).build();
    static final ZscriptCommandNode<DefaultResponse> cmd4 = new GenericCommandBuilder().setField('D', 0).build();

    @ParameterizedTest
    @MethodSource
    public void shouldUseConditionalConjunctions(final CommandSequenceNode input, final String output) {
        assertThat(input.asStringUtf8()).isEqualTo(output);
    }

    private static Stream<Arguments> shouldUseConditionalConjunctions() {
        return Stream.of(
                of(ifSucceeds(cmd1).then(cmd2).otherwise(cmd3), "A&(B|)|C"),
                of(ifFails(cmd1).then(cmd2).otherwise(cmd3), "A&(C|)|B"),

                of(ifSucceeds(new BlankCommandNode()).then(cmd2).otherwise(cmd3), "B"),
                of(ifFails(new BlankCommandNode()).then(cmd2).otherwise(cmd3), "C"),
                of(ifSucceeds(new AbortCommandNode()).then(cmd2).otherwise(cmd3), "Z1S13"),
                of(ifFails(new AbortCommandNode()).then(cmd2).otherwise(cmd3), "Z1S13"),
                of(ifSucceeds(new FailureCommandNode()).then(cmd2).otherwise(cmd3), "C"),
                of(ifFails(new FailureCommandNode()).then(cmd2).otherwise(cmd3), "B")
        );
    }

    @ParameterizedTest
    @MethodSource
    public void shouldUseLogicalConjunctions(final CommandSequenceNode input, final String output) {
        assertThat(input.asStringUtf8()).isEqualTo(output);
    }

    private static Stream<Arguments> shouldUseLogicalConjunctions() {
        return Stream.of(
                of(cmd1.andThen(cmd2).andThen(cmd3), "A&B&C"),
                of(cmd1.onFail(cmd2).andThen(cmd3), "(A|B)&C"),
                of(cmd1.andThen(cmd2).onFail(cmd3), "A&B|C"),
                of(cmd1.onFail(cmd2).onFail(cmd3), "A|B|C"),
                of(cmd1.thenFail().onFail(cmd3), "A&Z1S2|C"),
                of(cmd1.thenAbort().onFail(cmd3), "A&Z1S13|C"),
                of(cmd1.dropFailureCondition(), "A|"),

                of(new BlankCommandNode().andThen(cmd2).andThen(cmd3).abortOnFail().andThen(cmd4), "(B&C|Z1S13)&D"),
                of(new BlankCommandNode().onFail(cmd2).andThen(cmd3).abortOnFail().andThen(cmd4), "(C|Z1S13)&D"),
                of(new BlankCommandNode().andThen(cmd2).onFail(cmd3).abortOnFail().andThen(cmd4), "(B|C|Z1S13)&D"),
                of(new BlankCommandNode().onFail(cmd2).onFail(cmd3).abortOnFail().andThen(cmd4), "D"),
                of(new BlankCommandNode().thenFail(), "Z1S2"),
                of(new BlankCommandNode().thenAbort(), "Z1S13"),
                of(new BlankCommandNode().dropFailureCondition(), ""),

                of(new AbortCommandNode().andThen(cmd2).andThen(cmd3).abortOnFail().andThen(cmd4), "Z1S13"),
                of(new AbortCommandNode().onFail(cmd2).andThen(cmd3).abortOnFail().andThen(cmd4), "Z1S13"),
                of(new AbortCommandNode().andThen(cmd2).onFail(cmd3).abortOnFail().andThen(cmd4), "Z1S13"),
                of(new AbortCommandNode().onFail(cmd2).onFail(cmd3).abortOnFail().andThen(cmd4), "Z1S13"),
                of(new AbortCommandNode().thenFail(), "Z1S13"),
                of(new AbortCommandNode().thenAbort(), "Z1S13"),
                of(new AbortCommandNode().dropFailureCondition(), "Z1S13"),

                of(new FailureCommandNode().andThen(cmd2).andThen(cmd3).abortOnFail().andThen(cmd4), "Z1S13"),
                of(new FailureCommandNode().onFail(cmd2).andThen(cmd3).abortOnFail().andThen(cmd4), "(B&C|Z1S13)&D"),
                of(new FailureCommandNode().andThen(cmd2).onFail(cmd3).abortOnFail().andThen(cmd4), "(C|Z1S13)&D"),
                of(new FailureCommandNode().onFail(cmd2).onFail(cmd3).abortOnFail().andThen(cmd4), "(B|C|Z1S13)&D"),
                of(new FailureCommandNode().thenFail(), "Z1S2"),
                of(new FailureCommandNode().onFail(cmd1).thenAbort(), "A&Z1S13"),
                of(new FailureCommandNode().dropFailureCondition(), "")
        );
    }

    @ParameterizedTest
    @MethodSource
    public void shouldParseResponses(final ZscriptCommandNode<DefaultResponse> node) {
        final ZscriptFieldSet fieldSet = ZscriptFieldSet.fromList(emptyList(), List.of(fieldOf((byte) 'A', 12)));
        DefaultResponse       br       = node.parseResponse(fieldSet);
        assertThat(br.getField((byte) 'A')).hasValue(12);
        assertThat(node.getResponseType()).isEqualTo(DefaultResponse.class);
    }

    private static Stream<Arguments> shouldParseResponses() {
        return Stream.of(
                of(new GenericCommandBuilder().build()),
                of(new BlankCommandNode()),
                of(new AbortCommandNode()),
                of(new FailureCommandNode())
        );
    }

}
