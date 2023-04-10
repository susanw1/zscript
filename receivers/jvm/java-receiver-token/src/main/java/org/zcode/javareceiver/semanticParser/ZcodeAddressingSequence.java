package org.zcode.javareceiver.semanticParser;

import org.zcode.javareceiver.addressing.AddressingResult;
import org.zcode.javareceiver.core.ZcodeStatus;
import org.zcode.javareceiver.tokenizer.ZcodeTokenIterator;

public class ZcodeAddressingSequence {
    private final AddressingResult addrResult = new AddressingResult();

    public void run(ZcodeReceiverSequence seq) {
        if (addrResult.addressingResponse == AddressingResult.ADDRESSING_STARTED) {
            return;
        }
        if (addrResult.addressingResponse == AddressingResult.ADDRESSING_NOT_STARTED) {
            if (seq.hasFoundSeperator) {
                ZcodeTokenIterator iterator = new ZcodeTokenIterator();
                seq.buffer.setIterator(iterator);
                if (!iterator.isValid()) {
                    return;
                }
                seq.zcode.address(iterator, addrResult);
                addrResult.addressingResponse = AddressingResult.ADDRESSING_STARTED;
            }
        } else if (addrResult.addressingResponse == AddressingResult.ADDRESSING_SUCCESS) {
            seq.skipToNL();
        } else {
            seq.fail(ZcodeStatus.ADDRESSING_FAIL);
        }
    }

    public void reset() {
        addrResult.addressingResponse = AddressingResult.ADDRESSING_NOT_STARTED;
    }

    public boolean needsRunning() {
        return addrResult.addressingResponse != AddressingResult.ADDRESSING_STARTED;
    }
}
