package net.zscript.javareceiver.execution;

import java.util.Optional;

import net.zscript.javareceiver.core.OutStream;
import net.zscript.javareceiver.core.Zscript;
import net.zscript.javareceiver.core.ZscriptCommandOutStream;
import net.zscript.javareceiver.semanticParser.ContextView;
import net.zscript.model.components.Zchars;
import net.zscript.model.components.ZscriptStatus;
import net.zscript.tokenizer.BlockIterator;
import net.zscript.tokenizer.TokenBuffer.TokenReader.ReadToken;
import net.zscript.tokenizer.Tokenizer;
import net.zscript.util.OptIterator;

public class AddressingContext extends AbstractContext {
    private final Zscript zscript;

    public AddressingContext(final Zscript zscript, final ContextView contextView) {
        super(contextView);
        this.zscript = zscript;
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
        return new OptIterator<>() {
            final OptIterator<ReadToken> internal = contextView.getReader().iterator();

            @Override
            public Optional<Integer> next() {
                return internal.next().filter(rt -> Zchars.isAddressing(rt.getKey())).map(ReadToken::getData16);
            }
        };
    }

    @Override
    public boolean status(byte status) {
        OutStream               out        = zscript.getNotificationOutStream();
        ZscriptCommandOutStream commandOut = out.asCommandOutStream();
        if (!out.isOpen()) {
            out.open();
        }
        commandOut.writeField('!', 0);
        commandOut.writeField(Zchars.Z_STATUS, status);
        out.endSequence();
        out.close();
        return super.status(status);
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
            if (Zchars.isAddressing(token.getKey())) {
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
