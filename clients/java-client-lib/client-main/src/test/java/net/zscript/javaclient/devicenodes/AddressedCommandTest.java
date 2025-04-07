package net.zscript.javaclient.devicenodes;

import static net.zscript.javaclient.commandpaths.ZscriptAddress.address;
import static net.zscript.javaclient.tokens.ExtendingTokenBuffer.tokenize;
import static net.zscript.util.ByteString.byteStringUtf8;
import static org.assertj.core.api.Assertions.assertThat;

import org.junit.jupiter.api.Test;

import net.zscript.javaclient.sequence.CommandSequence;
import net.zscript.model.ZscriptModel;

class AddressedCommandTest {
    @Test
    public void shouldCreateAddressedCommand() {
        final CommandSequence  seq = parse("%10>2A3B4");
        final AddressedCommand cmd = new AddressedCommand(seq);
        cmd.prefixAddressLayer(address(10, 11));
        cmd.prefixAddressLayer(address(14));
        assertThat(cmd.asStringUtf8()).isEqualTo("@e@a.b%10>2A3B4\n");
        assertThat(seq).isSameAs(cmd.getCommandSequence());
        assertThat(cmd.hasAddress()).isTrue();
    }

    @Test
    public void shouldCreateAddressedCommandWithoutAddress() {
        final CommandSequence  seq = parse("A");
        final AddressedCommand cmd = new AddressedCommand(seq);
        assertThat(cmd.asStringUtf8()).isEqualTo("A\n");
        assertThat(seq).isSameAs(cmd.getCommandSequence());
        assertThat(cmd.hasAddress()).isFalse();
    }

    @Test
    public void shouldImplementToString() {
        final AddressedCommand cmd = new AddressedCommand(parse("A"));
        cmd.prefixAddressLayer(address(12, 13, 14));
        assertThat(cmd).hasToString("AddressedCommand:{'@c.d.eA\n'}");
    }

    private static CommandSequence parse(String s) {
        return CommandSequence.parse(ZscriptModel.standardModel(),
                tokenize(byteStringUtf8(s), true).getTokenReader().getFirstReadToken(), false);
    }
}
