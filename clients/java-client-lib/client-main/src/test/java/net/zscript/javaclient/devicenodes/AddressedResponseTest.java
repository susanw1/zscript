package net.zscript.javaclient.devicenodes;

import static net.zscript.javaclient.commandpaths.ZscriptAddress.address;
import static net.zscript.javaclient.tokens.ExtendingTokenBuffer.tokenize;
import static net.zscript.util.ByteString.byteStringUtf8;
import static org.assertj.core.api.Assertions.assertThat;
import static org.assertj.core.api.Assertions.assertThatThrownBy;

import org.junit.jupiter.api.Test;

import net.zscript.javaclient.commandpaths.CompleteAddressedResponse;

class AddressedResponseTest {
    private final AddressedResponse addressedResponse = new AddressedResponse(parse("@1.2 @ @3 !S"));

    @Test
    public void shouldCreateOffsetAddress() {
        assertThat(addressedResponse.getResponseSequence().asStringUtf8()).isEqualTo("!S");

        assertThat(addressedResponse.getAddressSection()).isEqualTo(address(1, 2));
        assertThat(addressedResponse.hasAddress()).isTrue();

        final AddressedResponse ar1 = addressedResponse.getChild();
        assertThat(ar1.getAddressSection()).isEqualTo(address(0));
        assertThat(ar1.hasAddress()).isTrue();

        final AddressedResponse ar2 = ar1.getChild();
        assertThat(ar2.getAddressSection()).isEqualTo(address(3));
        assertThat(ar2.hasAddress()).isTrue();

        final AddressedResponse ar3 = ar2.getChild();
        assertThat(ar3.hasAddress()).isFalse();

        assertThatThrownBy(ar3::getAddressSection)
                .isInstanceOf(IndexOutOfBoundsException.class).hasMessageContaining("out of bounds");
    }

    @Test
    public void shouldHandleEmptyAddress() {
        final AddressedResponse addressedResponse = new AddressedResponse(parse("!S"));
        assertThat(addressedResponse.hasAddress()).isFalse();

        assertThatThrownBy(addressedResponse::getAddressSection)
                .isInstanceOf(IndexOutOfBoundsException.class).hasMessageContaining("Index");
    }

    private static CompleteAddressedResponse parse(String s) {
        return CompleteAddressedResponse.parse(tokenize(byteStringUtf8(s)).getTokenReader().getFirstReadToken());
    }
}
