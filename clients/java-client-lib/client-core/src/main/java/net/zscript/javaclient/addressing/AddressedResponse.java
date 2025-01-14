package net.zscript.javaclient.addressing;

import net.zscript.javaclient.sequence.ResponseSequence;

/**
 * AddressedResponse that travels back through the device hierarchy aware of the current address section offset within the full address path.
 */
public class AddressedResponse {
    private final CompleteAddressedResponse addressedResponse;
    private final int                       offset;

    public AddressedResponse(CompleteAddressedResponse addressedResponse, int offset) {
        this.addressedResponse = addressedResponse;
        this.offset = offset;
    }

    public ZscriptAddress getAddressSection() {
        return addressedResponse.getAddressSection(offset);
    }

    public boolean hasAddress() {
        return addressedResponse.hasAddress(offset);
    }

    public ResponseSequence getResponseSequence() {
        return addressedResponse.getResponseSequence();
    }

    public AddressedResponse getChild() {
        return new AddressedResponse(addressedResponse, offset + 1);
    }
}
