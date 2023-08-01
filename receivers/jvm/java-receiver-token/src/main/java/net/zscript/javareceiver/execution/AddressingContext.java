package net.zscript.javareceiver.execution;

import java.util.Optional;

import net.zscript.javareceiver.core.ZscriptStatus;
import net.zscript.javareceiver.semanticParser.ContextView;
import net.zscript.javareceiver.tokenizer.BlockIterator;
import net.zscript.javareceiver.tokenizer.OptIterator;
import net.zscript.javareceiver.tokenizer.Zchars;
import net.zscript.javareceiver.tokenizer.Tokenizer;
import net.zscript.javareceiver.tokenizer.TokenBuffer.TokenReader.ReadToken;

public class AddressingContext extends AbstractContext {
    public AddressingContext(final ContextView contextView) {
        super(contextView);
    }

    public BlockIterator getAddressedData() {
        OptIterator<ReadToken> iterator = contextView.getReader().iterator();
        for (Optional<ReadToken> opt = iterator.next(); opt.isPresent(); opt = iterator.next()) {
            ReadToken token = opt.get();
            if (token.getKey() == Tokenizer.ADDRESSING_FIELD_KEY) {
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

    public int getAddressedDataSize() {
        final OptIterator<ReadToken> it = contextView.getReader().iterator();
        for (Optional<ReadToken> opt = it.next(); opt.isPresent(); opt = it.next()) {
            ReadToken token = opt.get();
            if (token.getKey() == Tokenizer.ADDRESSING_FIELD_KEY) {
                return token.getDataSize();
            }
        }
        return 0;
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
                status(ZscriptStatus.INVALID_KEY);
                return false;
            }
            if (token.getKey() == Zchars.Z_ADDRESSING || token.getKey() == Zchars.Z_ADDRESSING_CONTINUE) {
                continue;
            }
            if (token.getKey() != Tokenizer.ADDRESSING_FIELD_KEY) {
                status(ZscriptStatus.INVALID_KEY);
                return false;
            }
            if (i == 1) {
                status(ZscriptStatus.INTERNAL_ERROR);
                return false;
            }
            hasReachedData = true;
        }
        status(ZscriptStatus.MISSING_KEY);
        return false;
    }

}
