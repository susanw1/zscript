package net.zscript.javaclient.addressing;

import java.util.List;
import java.util.Optional;

import net.zscript.javaclient.ZscriptParseException;
import net.zscript.javaclient.sequence.ResponseSequence;
import net.zscript.model.components.Zchars;
import net.zscript.tokenizer.TokenBuffer.TokenReader.ReadToken;
import net.zscript.tokenizer.Tokenizer;

/**
 * Defines a Response sequence along with its address - this is the completed parse from a TokenReader.
 */
public class CompleteAddressedResponse {
    private final List<ZscriptAddress> addressSections;
    private final ResponseSequence     responseSequence;

    public static CompleteAddressedResponse parse(ReadToken start) {
        final Optional<ReadToken> endToken = start.tokenIterator().stream()
                .filter(ReadToken::isSequenceEndMarker).findFirst();
        if (endToken.isEmpty()) {
            throw new ZscriptParseException("Parse failed, no terminating sequence marker");
        } else if (endToken.get().getKey() != Tokenizer.NORMAL_SEQUENCE_END) {
            throw new ZscriptParseException("Parse failed with Tokenizer error [marker=%s]", endToken.get());
        }

        final List<ZscriptAddress> addresses = ZscriptAddress.parseAll(start);
        final ResponseSequence seq = start.tokenIterator().stream()
                .filter(t -> !Zchars.isAddressing(t.getKey()))
                .findFirst()
                .map(ResponseSequence::parse)
                .orElseGet(ResponseSequence::empty);

        return new CompleteAddressedResponse(addresses, seq);
    }

    private CompleteAddressedResponse(List<ZscriptAddress> addressSections, ResponseSequence responseSequence) {
        this.addressSections = addressSections;
        this.responseSequence = responseSequence;
    }

    public ZscriptAddress getAddressSection(int offset) {
        return addressSections.get(offset);
    }

    public boolean hasAddress(int offset) {
        return offset < addressSections.size();
    }

    public ResponseSequence getResponseSequence() {
        return responseSequence;
    }

    public AddressedResponse asResponse() {
        return new AddressedResponse(this, 0);
    }
}
