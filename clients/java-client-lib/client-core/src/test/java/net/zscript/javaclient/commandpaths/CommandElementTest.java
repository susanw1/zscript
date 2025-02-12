package net.zscript.javaclient.commandpaths;

import java.util.List;
import java.util.Map;

import static net.zscript.javaclient.commandpaths.ZscriptFieldSet.fromMap;
import static net.zscript.util.ByteString.byteStringUtf8;
import static org.assertj.core.api.Assertions.assertThat;
import static org.mockito.Mockito.mock;

import org.junit.jupiter.api.Test;

import net.zscript.model.components.Zchars;
import net.zscript.model.components.ZscriptStatus;

class CommandElementTest {
    private final ZscriptFieldSet testFieldSet = fromMap(List.of(new BigField(byteStringUtf8("hello"), true)), Map.of((byte) 'A', 6));

    @Test
    public void shouldCreateBasicElement() {
        final CommandElement cmd = new CommandElement(null, null, testFieldSet);
        assertThat(cmd.getFields()).isSameAs(testFieldSet);
        assertThat(cmd.getOnSuccess()).isNull();
        assertThat(cmd.getOnFail()).isNull();
        assertThat(cmd.asStringUtf8()).isEqualTo("A6\"hello\"");
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
        assertThat(cmd.asStringUtf8()).isEqualTo("A6\"hello\"");
        assertThat(cmd.canSucceed()).isTrue();
        assertThat(cmd.canFail()).isTrue();
    }

    @Test
    public void shouldDetermineCanSucceedForSucceedingEchoCommand() {
        final ZscriptFieldSet okCommand = fromMap(List.of(), Map.of(Zchars.Z_CMD, 1, Zchars.Z_STATUS, (int) ZscriptStatus.SUCCESS));

        final CommandElement cmd = new CommandElement(null, null, okCommand);
        assertThat(cmd.getFields()).isSameAs(okCommand);
        assertThat(cmd.asStringUtf8()).isEqualTo("Z1S");
        assertThat(cmd.canSucceed()).isTrue();
        assertThat(cmd.canFail()).isFalse();
    }

    @Test
    public void shouldDetermineCanSucceedForFailingEchoCommand() {
        final ZscriptFieldSet failCommand = fromMap(List.of(), Map.of(Zchars.Z_CMD, 1, Zchars.Z_STATUS, (int) ZscriptStatus.COMMAND_FAIL));

        final CommandElement cmd = new CommandElement(null, null, failCommand);
        assertThat(cmd.getFields()).isSameAs(failCommand);
        assertThat(cmd.asStringUtf8()).isEqualTo("Z1S1");
        assertThat(cmd.canSucceed()).isFalse();
        assertThat(cmd.canFail()).isTrue();
    }

    @Test
    public void shouldDetermineCanSucceedForAbortingEchoCommand() {
        final ZscriptFieldSet failCommand = fromMap(List.of(), Map.of(Zchars.Z_CMD, 1, Zchars.Z_STATUS, (int) ZscriptStatus.COMMAND_ERROR_CONTROL));

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
        assertThat(cmd1).hasToString("CommandElement:{'A6\"hello\"'}");
    }
}
