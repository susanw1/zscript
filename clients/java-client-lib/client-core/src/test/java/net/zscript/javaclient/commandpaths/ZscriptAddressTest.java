package net.zscript.javaclient.commandpaths;

import java.util.List;

import static net.zscript.javaclient.commandpaths.ZscriptAddress.address;
import static net.zscript.util.ByteString.byteStringUtf8;
import static org.assertj.core.api.Assertions.assertThat;
import static org.assertj.core.api.Assertions.assertThatIllegalArgumentException;

import nl.jqno.equalsverifier.EqualsVerifier;
import org.junit.jupiter.api.Test;

import net.zscript.javaclient.tokens.ExtendingTokenBuffer;
import net.zscript.tokenizer.TokenBuffer.TokenReader.ReadToken;
import net.zscript.util.ByteString;

class ZscriptAddressTest {
    @Test
    public void shouldConstructFromNumericParts() {
        ZscriptAddress address1 = ZscriptAddress.from(1, 0xa3b, 0, 0xffff);
        assertThat(address1.size()).isEqualTo(4);
        assertThat(address1.asStringUtf8()).isEqualTo("@1.a3b..ffff");

        ZscriptAddress address2 = ZscriptAddress.from(List.of(1, 0xa3b, 0, 0xffff));
        assertThat(address2.size()).isEqualTo(4);
        assertThat(address2.asStringUtf8()).isEqualTo("@1.a3b..ffff");

        assertThat(address1).isEqualTo(address2);
    }

    @Test
    public void shouldParseSimpleAddress() {
        final ReadToken            token = ExtendingTokenBuffer.tokenize(byteStringUtf8("@12.34.ef Z1A2+34")).getTokenReader().getFirstReadToken();
        final List<ZscriptAddress> a     = ZscriptAddress.parseAll(token);
        assertThat(a).hasSize(1);
        assertThat(a.get(0).getAsInts()).containsExactly(0x12, 0x34, 0xef);
    }

    /**
     * This test is a bit of a trick one - it's actually the Tokenizer that treats everything after the first address as "stuff inside the envelope" and puts it all in one big
     * token. That needs re-parsing in order to see the next level address.
     */
    @Test
    public void shouldParseMultipleAddresses() {
        final ReadToken token = ExtendingTokenBuffer.tokenize(byteStringUtf8("@12.34.56 @78.ab Z1A2+34")).getTokenReader().getFirstReadToken();

        List<ZscriptAddress> addresses = ZscriptAddress.parseAll(token);

        assertThat(addresses).hasSize(2);
        assertThat(addresses.get(0).getAsInts()).containsExactly(0x12, 0x34, 0x56);
        assertThat(addresses.get(1).getAsInts()).containsExactly(0x78, 0xab);
    }

    @Test
    public void shouldNotParseNonAddresses() {
        final ReadToken token = ExtendingTokenBuffer.tokenize(byteStringUtf8("A\n")).getTokenReader().getFirstReadToken();
        assertThat(ZscriptAddress.parseAll(token)).isEmpty();
    }

    @Test
    public void shouldFailOnOutOfRangeAddresses() {
        assertThatIllegalArgumentException().isThrownBy(() -> ZscriptAddress.from(1, 0x12345));
        assertThatIllegalArgumentException().isThrownBy(() -> ZscriptAddress.from(1, -1));
    }

    @Test
    public void shouldWriteToByteString() {
        ZscriptAddress address = address(2, 1);
        assertThat(address.toByteString())
                .isEqualTo(ByteString.concat((object, builder) -> builder.appendUtf8(object), "@2.1"));
    }

    @Test
    public void shouldGetBufferLength() {
        // Number of bytes required in the TokenBuffer - 2 (tokens' key/len) + (0, 1, or 2) bytes, summed for each part.
        ZscriptAddress address = address(1, 0xa3b, 0, 0xffff);
        // (a) 2+1, (b) 2+2, (c) 2+0, (d) 2+2   = 13
        assertThat(address.getBufferLength()).isEqualTo(13);
    }

    @Test
    public void shouldImplementEquals() {
        EqualsVerifier.forClass(ZscriptAddress.class).verify();
    }

    @Test
    public void shouldImplementToString() {
        assertThat(address(1, 0xa3b, 0, 0xffff)).hasToString("ZscriptAddress:{'@1.a3b..ffff'}");
    }
}
