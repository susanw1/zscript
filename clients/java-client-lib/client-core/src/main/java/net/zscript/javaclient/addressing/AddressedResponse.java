package net.zscript.javaclient.addressing;

import net.zscript.javaclient.sequence.ResponseSequence;

public class AddressedResponse {
    private final CompleteAddressedResponse response;
    private final int                       offset;

    public AddressedResponse(CompleteAddressedResponse response, int offset) {
        this.response = response;
        this.offset = offset;
    }

    public ZscriptAddress getAddressSection() {
        return response.getAddressSection(offset);
    }

    public boolean hasAddress() {
        return response.hasAddress(offset);
    }

    public ResponseSequence getContent() {
        return response.getContent();
    }

    public AddressedResponse getChild() {
        return new AddressedResponse(response, offset + 1);
    }
}
