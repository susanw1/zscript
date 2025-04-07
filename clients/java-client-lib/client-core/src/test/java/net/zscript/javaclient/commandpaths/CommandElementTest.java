package net.zscript.javaclient.commandpaths;

import java.util.List;

import static net.zscript.javaclient.commandpaths.FieldElement.fieldOf;
import static net.zscript.javaclient.commandpaths.FieldElement.fieldOfText;
import static net.zscript.javaclient.commandpaths.ZscriptFieldSet.fromList;
import static net.zscript.util.ByteString.byteStringUtf8;
import static org.assertj.core.api.Assertions.assertThat;
import static org.mockito.Mockito.mock;

import org.junit.jupiter.api.Test;

import net.zscript.model.components.Zchars;
import net.zscript.model.components.ZscriptStatus;

class CommandElementTest {
    private final ZscriptFieldSet testFieldSet = fromList(List.of(fieldOf((byte) 'A', 6), fieldOfText((byte) 'B', byteStringUtf8("hello"))));

    @Test
    public void shouldCreateBasicElement() {
        final CommandElement cmd = new CommandElement(null, null, testFieldSet);
        assertThat(cmd.getFields()).isSameAs(testFieldSet);
        assertThat(cmd.getOnSuccess()).isNull();
        assertThat(cmd.getOnFail()).isNull();
        assertThat(cmd.asStringUtf8()).isEqualTo("A6B\"hello\"");
        assertThat(cmd.canSucceed()).isTrue();
        assertThat(cmd.canFail()).isTrue();
    }

    @Test
    public void shouldCreateElementWithNextReferences() {
        final CommandElement cmd1 = mock(CommandElement.class);
        final CommandElement cmd2 = mock(CommandElement.class);
        final CommandElement cmd  = new CommandElement(cmd1, cmd2, testFieldSet);
        assertThat(cmd.getFields()).isSameAs(testFieldSet);
        assertThat(cmd.getOnSuccess()).isSameAs(cmd1);
        assertThat(cmd.getOnFail()).isSameAs(cmd2);
        assertThat(cmd.asStringUtf8()).isEqualTo("A6B\"hello\"");
        assertThat(cmd.canSucceed()).isTrue();
        assertThat(cmd.canFail()).isTrue();
    }

    @Test
    public void shouldDetermineCanSucceedForSucceedingEchoCommand() {
        final ZscriptFieldSet okCommand = fromList(List.of(fieldOf(Zchars.Z_CMD, 1), fieldOf(Zchars.Z_STATUS, ZscriptStatus.SUCCESS)));

        final CommandElement cmd = new CommandElement(null, null, okCommand);
        assertThat(cmd.getFields()).isSameAs(okCommand);
        assertThat(cmd.asStringUtf8()).isEqualTo("Z1S");
        assertThat(cmd.canSucceed()).isTrue();
        assertThat(cmd.canFail()).isFalse();
    }

    @Test
    public void shouldDetermineCanSucceedForFailingEchoCommand() {
        final ZscriptFieldSet failCommand = fromList(List.of(fieldOf(Zchars.Z_CMD, 1), fieldOf(Zchars.Z_STATUS, ZscriptStatus.COMMAND_FAIL)));

        final CommandElement cmd = new CommandElement(null, null, failCommand);
        assertThat(cmd.getFields()).isSameAs(failCommand);
        assertThat(cmd.asStringUtf8()).isEqualTo("Z1S1");
        assertThat(cmd.canSucceed()).isFalse();
        assertThat(cmd.canFail()).isTrue();
    }

    @Test
    public void shouldDetermineCanSucceedForAbortingEchoCommand() {
        final ZscriptFieldSet failCommand = fromList(List.of(fieldOf(Zchars.Z_CMD, 1), fieldOf(Zchars.Z_STATUS, ZscriptStatus.COMMAND_ERROR_CONTROL)));

        final CommandElement cmd = new CommandElement(null, null, failCommand);
        assertThat(cmd.getFields()).isSameAs(failCommand);
        assertThat(cmd.asStringUtf8()).isEqualTo("Z1S13");
        assertThat(cmd.canSucceed()).isFalse();
        assertThat(cmd.canFail()).isFalse();
    }

    @Test
    public void shouldImplementEntityBasedEqualsAndHashCode() {
        final CommandElement cmd1 = new CommandElement(null, null, testFieldSet);
        final CommandElement cmd2 = new CommandElement(null, null, testFieldSet);
        assertThat(cmd1.equals(cmd2)).isFalse();
        assertThat(cmd1.hashCode()).isEqualTo(System.identityHashCode(cmd1));
    }

    @Test
    public void shouldImplementToString() {
        final CommandElement cmd1 = new CommandElement(null, null, testFieldSet);
        assertThat(cmd1).hasToString("CommandElement:{'A6B\"hello\"'}");
    }
}
