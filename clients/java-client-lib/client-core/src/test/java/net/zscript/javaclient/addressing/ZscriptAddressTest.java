package net.zscript.javaclient.addressing;

import java.nio.charset.StandardCharsets;
import java.util.List;

import static java.util.stream.Collectors.toList;
import static org.assertj.core.api.Assertions.assertThat;
import static org.assertj.core.api.Assertions.assertThatIllegalArgumentException;

import nl.jqno.equalsverifier.EqualsVerifier;
import org.junit.jupiter.api.Test;

import net.zscript.javareceiver.tokenizer.TokenBuffer;
import net.zscript.javareceiver.tokenizer.TokenBuffer.TokenReader;
import net.zscript.javareceiver.tokenizer.TokenBuffer.TokenReader.ReadToken;
import net.zscript.javareceiver.tokenizer.TokenRingBuffer;
import net.zscript.javareceiver.tokenizer.Tokenizer;
import net.zscript.model.components.Zchars;
import net.zscript.util.ByteString;

class ZscriptAddressTest {
    TokenBuffer buffer      = TokenRingBuffer.createBufferWithCapacity(256);
    Tokenizer   tokenizer   = new Tokenizer(buffer.getTokenWriter(), 2);
    TokenReader tokenReader = buffer.getTokenReader();

    @Test
    public void shouldConstructFromNumericParts() {
        ZscriptAddress address1 = ZscriptAddress.from(1, 0xa3b, 0, 0xffff);
        assertThat(address1.size()).isEqualTo(4);
        assertThat(address1.toString()).isEqualTo("@1.a3b..ffff");

        ZscriptAddress address2 = ZscriptAddress.from(List.of(1, 0xa3b, 0, 0xffff));
        assertThat(address2.size()).isEqualTo(4);
        assertThat(address2.toString()).isEqualTo("@1.a3b..ffff");

        assertThat(address1).isEqualTo(address2);

        EqualsVerifier.forClass(ZscriptAddress.class).verify();
    }

    @Test
    public void shouldParseSimpleAddress() {
        tokenize("@12.34.ef Z1A2+34\n");
        ReadToken            token = tokenReader.getFirstReadToken();
        final ZscriptAddress a     = ZscriptAddress.parse(token);
        assertThat(a.getAsInts()).containsExactly(0x12, 0x34, 0xef);
    }

    /**
     * This test is a bit of a trick one - it's actually the Tokenizer that treats everything after the first address as "stuff inside the envelope" and puts it all in one big
     * token. That needs re-parsing in order to see the next level address.
     */
    @Test
    public void shouldNotParseMultipleAddresses() {
        tokenize("@12.34.56 @78.ab Z1A2+34\n");

        List<ZscriptAddress> addresses = tokenReader.iterator().stream()
                .filter(t -> t.getKey() == Zchars.Z_ADDRESSING)
                .map(ZscriptAddress::parse)
                .collect(toList());

        // tokenReader.iterator().stream().forEach(System.out::println);

        assertThat(addresses).hasSize(1);
        assertThat(addresses.get(0).getAsInts()).containsExactly(0x12, 0x34, 0x56);
    }

    @Test
    public void shouldNotParseNonAddresses() {
        tokenize("A\n");
        ReadToken token = tokenReader.getFirstReadToken();
        assertThatIllegalArgumentException().isThrownBy(() -> ZscriptAddress.parse(token));
    }

    @Test
    public void shouldFailOnOutOfRangeAddresses() {
        assertThatIllegalArgumentException().isThrownBy(() -> ZscriptAddress.from(1, 0x12345));
    }

    @Test
    public void shouldWriteToByteString() {
        ZscriptAddress address = ZscriptAddress.address(2, 1);
        assertThat(address.toByteString())
                .isEqualTo(ByteString.concat((object, builder) -> builder.appendUtf8(object), "@2.1"));
    }

    @Test
    public void shouldGetBufferLength() {
        // Number of bytes required in the TokenBuffer - 2 (tokens' key/len) + (0, 1, or 2) bytes, summed for each part.
        ZscriptAddress address = ZscriptAddress.address(1, 0xa3b, 0, 0xffff);
        // (a) 2+1, (b) 2+2, (c) 2+0, (d) 2+2   = 13
        assertThat(address.getBufferLength()).isEqualTo(13);
    }

    private void tokenize(String z) {
        for (byte c : z.getBytes(StandardCharsets.ISO_8859_1)) {
            tokenizer.accept(c);
        }
    }
}
