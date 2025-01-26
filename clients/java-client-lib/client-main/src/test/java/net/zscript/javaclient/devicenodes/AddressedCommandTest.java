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
        final CommandSequence  seq = parse("%1_2A3B4");
        final AddressedCommand cmd = new AddressedCommand(seq);
        cmd.addAddressLayer(address(10, 11));
        cmd.addAddressLayer(address(14));
        assertThat(cmd.asStringUtf8()).isEqualTo("@a.b@e%10_2A3B4\n");
        assertThat(seq).isSameAs(cmd.getCommandSequence());
        assertThat(cmd.hasAddressLayer()).isTrue();
    }

    @Test
    public void shouldCreateAddressedCommandWithoutAddress() {
        final CommandSequence  seq = parse("A");
        final AddressedCommand cmd = new AddressedCommand(seq);
        assertThat(cmd.asStringUtf8()).isEqualTo("A\n");
        assertThat(seq).isSameAs(cmd.getCommandSequence());
        assertThat(cmd.hasAddressLayer()).isFalse();
    }

    @Test
    public void shouldImplementToString() {
        final AddressedCommand cmd = new AddressedCommand(parse("A"));
        cmd.addAddressLayer(address(12, 13, 14));
        assertThat(cmd).hasToString("AddressedCommand:{'@c.d.eA\n'}");
    }

    private static CommandSequence parse(String s) {
        return CommandSequence.parse(ZscriptModel.standardModel(),
                tokenize(byteStringUtf8(s), true).getTokenReader().getFirstReadToken(), false);
    }
}
