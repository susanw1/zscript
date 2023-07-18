package org.zcode.javareceiver.execution;

import java.util.Optional;

import org.zcode.javareceiver.core.ZcodeStatus;
import org.zcode.javareceiver.semanticParser.ContextView;
import org.zcode.javareceiver.tokenizer.BlockIterator;
import org.zcode.javareceiver.tokenizer.OptIterator;
import org.zcode.javareceiver.tokenizer.Zchars;
import org.zcode.javareceiver.tokenizer.ZcodeTokenBuffer.TokenReader.ReadToken;
import org.zcode.javareceiver.tokenizer.ZcodeTokenizer;

public class ZcodeAddressingContext {
    private final ContextView contextView;

    public ZcodeAddressingContext(ContextView contextView) {
        this.contextView = contextView;
    }

    public BlockIterator getAddressedData() {
        OptIterator<ReadToken> iterator = contextView.getReader().iterator();
        for (Optional<ReadToken> opt = iterator.next(); opt.isPresent(); opt = iterator.next()) {
            ReadToken token = opt.get();
            if (token.getKey() == ZcodeTokenizer.ADDRESSING_FIELD_KEY) {
                return token.blockIterator();
            }
        }
        throw new IllegalStateException("No data with addressing");
    }

    public OptIterator<Integer> getAddressSegments() {
        return new OptIterator<Integer>() {
            OptIterator<ReadToken> internal = contextView.getReader().iterator();

            @Override
            public Optional<Integer> next() {
                return internal.next().filter(rt -> rt.getKey() == Zchars.Z_ADDRESSING || rt.getKey() == Zchars.Z_ADDRESSING_CONTINUE).map(rt -> rt.getData16());
            }
        };
    }

    public void status(byte status) {
        contextView.setStatus(status);
//        if (!ZcodeStatus.isSuccess(status)) {
//            if (ZcodeStatus.isError(status)) {
//                parser.error();
//            } else {
//                parser.softFail();
//            }
//            if (!out.isOpen()) {
//                out.open();
//            }
//            out.writeField('S', status);
//            out.endSequence();
//            out.close();
//        }
    }

    public boolean verify() {
        boolean hasReachedData = false;
        int     i              = 0;

        OptIterator<ReadToken> iterator = contextView.getReader().iterator();
        for (Optional<ReadToken> opt = iterator.next(); opt.isPresent(); opt = iterator.next()) {
            ReadToken token = opt.get();
            i++;
            if (hasReachedData) {
                if (token.isMarker()) {
                    return true;
                }
                status(ZcodeStatus.INVALID_KEY);
                return false;
            }
            if (token.getKey() == Zchars.Z_ADDRESSING || token.getKey() == Zchars.Z_ADDRESSING_CONTINUE) {
                continue;
            }
            if (token.getKey() != ZcodeTokenizer.ADDRESSING_FIELD_KEY) {
                status(ZcodeStatus.INVALID_KEY);
                return false;
            }
            if (i == 1) {
                status(ZcodeStatus.INTERNAL_ERROR);
                return false;
            }
            hasReachedData = true;
        }
        status(ZcodeStatus.MISSING_KEY);
        return false;
    }

    public void setCommandCanComplete() {
        contextView.setCommandCanComplete(true);
    }

    public void clearCommandCanComplete() {
        contextView.setCommandCanComplete(false);
    }

    public boolean commandCanComplete() {
        return contextView.commandCanComplete();
    }

    public boolean isActivated() {
        return contextView.isActivated();
    }
}
