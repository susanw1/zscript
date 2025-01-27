package net.zscript.javaclient.devicenodes;

import static net.zscript.javaclient.commandpaths.ZscriptAddress.address;
import static net.zscript.javaclient.tokens.ExtendingTokenBuffer.tokenize;
import static net.zscript.util.ByteString.byteStringUtf8;
import static org.assertj.core.api.Assertions.assertThat;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.verify;

import org.junit.jupiter.api.Test;

import net.zscript.javaclient.commandpaths.ZscriptAddress;
import net.zscript.javaclient.sequence.CommandSequence;
import net.zscript.model.ZscriptModel;

class AddressingConnectionTest {
    final Connection            parentConnection = mock(Connection.class);
    final ResponseMatchListener matchListener    = mock(ResponseMatchListener.class);

    @Test
    public void shouldContainAddressOnSend() {
        final ZscriptAddress       address              = address(1, 2);
        final AddressingConnection addressingConnection = new AddressingConnection(address, parentConnection, matchListener);

        final AddressedCommand cmd = parse("Z0");
        addressingConnection.send(cmd);
        assertThat(cmd.hasAddress()).isTrue();
        assertThat(cmd.asStringUtf8()).isEqualTo("@1.2Z\n");
        assertThat(cmd.getCommandSequence().asStringUtf8()).isEqualTo("Z");
        assertThat(cmd.getAddressSections()).containsExactly(address);

        verify(parentConnection).send(cmd);
    }

    @Test
    public void shouldPrefixNewAddressSection() {
        /* If we send a command to a sub-device, it has to go via the parent towards the root of the connection tree. We expect to see the addresses
         prefixed to the front of the address so that they look like: @<main> @<sub> @<sub-sub> etc, because that's how they are processed on the target. */
        final ZscriptAddress       mainAddress = address(1, 2);
        final AddressingConnection mainConn    = new AddressingConnection(mainAddress, parentConnection, matchListener);
        final ZscriptAddress       subAddress  = address(3);
        final AddressingConnection subConn     = new AddressingConnection(subAddress, mainConn, matchListener);

        final AddressedCommand cmd = parse("Z0");
        subConn.send(cmd);

        assertThat(cmd.hasAddress()).isTrue();
        assertThat(cmd.asStringUtf8()).isEqualTo("@1.2@3Z\n");
        assertThat(cmd.getCommandSequence().asStringUtf8()).isEqualTo("Z");
        assertThat(cmd.getAddressSections()).containsExactly(mainAddress, subAddress);

        verify(parentConnection).send(cmd);
    }

    @Test
    public void shouldReceive() {
        final AddressingConnection mainConn = new AddressingConnection(address(1, 2), parentConnection, matchListener);
        final AddressedResponse    addrResp = mock(AddressedResponse.class);
        mainConn.response(addrResp);

        mainConn.onReceive(r -> {
            assertThat(r).isSameAs(addrResp);
        });
        mainConn.response(addrResp);
    }

    @Test
    public void shouldNotifyListenersOfMatch() {
        final AddressingConnection mainConn = new AddressingConnection(address(1, 2), parentConnection, matchListener);
        final AddressedCommand     cmd      = parse("Z0");
        mainConn.responseHasMatched(cmd);

        verify(parentConnection).responseHasMatched(cmd);
        verify(matchListener).responseHasMatched(cmd);
    }

    @Test
    public void shouldGetAssociatedThread() {
        final AddressingConnection mainConn = new AddressingConnection(address(1, 2), parentConnection, matchListener);
        mainConn.getAssociatedThread();
        verify(parentConnection).getAssociatedThread();
    }

    private static AddressedCommand parse(String s) {
        return new AddressedCommand(CommandSequence.parse(ZscriptModel.standardModel(),
                tokenize(byteStringUtf8(s), true).getTokenReader().getFirstReadToken(), false));
    }
}
