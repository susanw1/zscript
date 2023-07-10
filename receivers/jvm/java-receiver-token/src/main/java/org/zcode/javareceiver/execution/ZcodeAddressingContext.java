package org.zcode.javareceiver.execution;

import java.util.Optional;

import org.zcode.javareceiver.core.ZcodeOutStream;
import org.zcode.javareceiver.core.ZcodeStatus;
import org.zcode.javareceiver.semanticParser.SemanticParser;
import org.zcode.javareceiver.tokenizer.BlockIterator;
import org.zcode.javareceiver.tokenizer.OptIterator;
import org.zcode.javareceiver.tokenizer.Zchars;
import org.zcode.javareceiver.tokenizer.ZcodeTokenBuffer.TokenReader.ReadToken;
import org.zcode.javareceiver.tokenizer.ZcodeTokenizer;

public class ZcodeAddressingContext {
    private final SemanticParser parser;
    private final ZcodeOutStream out;

    public ZcodeAddressingContext(SemanticParser parser, ZcodeOutStream out) {
        this.parser = parser;
        this.out = out;
    }

    public BlockIterator getAddressedData() {
        OptIterator<ReadToken> iterator = parser.getReader().iterator();
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
            OptIterator<ReadToken> internal = parser.getReader().iterator();

            @Override
            public Optional<Integer> next() {
                return internal.next().filter(rt -> rt.getKey() == Zchars.Z_ADDRESSING || rt.getKey() == Zchars.Z_ADDRESSING_CONTINUE).map(rt -> rt.getData16());
            }
        };
    }

    public void status(byte status) {
        if (!ZcodeStatus.isSuccess(status)) {
            if (ZcodeStatus.isSystemError(status)) {
                parser.error();
            } else {
                parser.softFail();
            }
            if (!out.isOpen()) {
                out.open();
            }
            out.writeField('S', status);
            out.endSequence();
            out.close();
        }
    }

    public boolean verify() {
        boolean hasReachedData = false;
        int     i              = 0;

        OptIterator<ReadToken> iterator = parser.getReader().iterator();
        for (Optional<ReadToken> opt = iterator.next(); opt.isPresent(); opt = iterator.next()) {
            ReadToken token = opt.get();
            i++;
            if (hasReachedData) {
                if (token.isMarker()) {
                    return true;
                }
                status(ZcodeStatus.TOKEN_ERROR);
                return false;
            }
            if (token.getKey() == Zchars.Z_ADDRESSING || token.getKey() == Zchars.Z_ADDRESSING_CONTINUE) {
                continue;
            }
            if (token.getKey() != ZcodeTokenizer.ADDRESSING_FIELD_KEY) {
                status(ZcodeStatus.TOKEN_ERROR);
                return false;
            }
            if (i == 1) {
                status(ZcodeStatus.TOKEN_ERROR);
                return false;
            }
            hasReachedData = true;
        }
        status(ZcodeStatus.TOKEN_ERROR);
        return false;
    }

    public void setComplete() {
        parser.setComplete(true);
    }

    public void clearComplete() {
        parser.setComplete(false);
    }

    public boolean isActivated() {
        return parser.isActivated();
    }

    public boolean isComplete() {
        return parser.isComplete();
    }

}
