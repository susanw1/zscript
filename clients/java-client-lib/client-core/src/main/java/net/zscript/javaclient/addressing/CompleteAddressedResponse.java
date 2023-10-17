package net.zscript.javaclient.addressing;

import static net.zscript.javareceiver.tokenizer.TokenBuffer.TokenReader;
import static net.zscript.javareceiver.tokenizer.TokenBuffer.TokenReader.ReadToken;

import java.util.ArrayList;
import java.util.List;
import java.util.Optional;

import net.zscript.javaclient.sequence.ResponseSequence;
import net.zscript.model.components.Zchars;
import net.zscript.util.OptIterator;

public class CompleteAddressedResponse {
    private final List<ZscriptAddress> addressSections;
    private final ResponseSequence     content;

    public static CompleteAddressedResponse parse(TokenReader reader) {
        OptIterator<ReadToken> iter      = reader.iterator();
        List<ZscriptAddress>   addresses = new ArrayList<>();
        ResponseSequence       seq       = null;
        for (Optional<ReadToken> opt = iter.next(); opt.isPresent(); opt = iter.next()) {
            ReadToken token = opt.get();
            if (token.getKey() == Zchars.Z_ADDRESSING) {
                addresses.add(ZscriptAddress.parse(token));
            } else {
                seq = ResponseSequence.parse(token);
                break;
            }
        }
        if (seq == null) {
            seq = ResponseSequence.parse(null);
        }
        return new CompleteAddressedResponse(addresses, seq);
    }

    private CompleteAddressedResponse(List<ZscriptAddress> addressSections, ResponseSequence content) {
        this.addressSections = addressSections;
        this.content = content;
    }

    public ZscriptAddress getAddressSection(int offset) {
        return addressSections.get(offset);
    }

    public boolean hasAddress(int offset) {
        return addressSections.size() > offset;
    }

    public ResponseSequence getContent() {
        return content;
    }

    public AddressedResponse asResponse() {
        return new AddressedResponse(this, 0);
    }
}

