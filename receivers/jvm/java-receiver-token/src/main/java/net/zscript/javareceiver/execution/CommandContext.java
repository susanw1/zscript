package net.zscript.javareceiver.execution;

import java.util.BitSet;
import java.util.Optional;
import java.util.OptionalInt;

import net.zscript.javareceiver.core.OutStream;
import net.zscript.javareceiver.core.Zscript;
import net.zscript.javareceiver.core.ZscriptCommandOutStream;
import net.zscript.javareceiver.core.ZscriptStatus;
import net.zscript.javareceiver.semanticParser.ContextView;
import net.zscript.javareceiver.tokenizer.BlockIterator;
import net.zscript.javareceiver.tokenizer.TokenBuffer.TokenReader.ReadToken;
import net.zscript.javareceiver.tokenizer.ZscriptExpression;
import net.zscript.javareceiver.tokenizer.ZscriptTokenExpression;
import net.zscript.model.components.Zchars;
import net.zscript.util.OptIterator;

public class CommandContext extends AbstractContext implements ZscriptExpression {
    private final Zscript                zscript;
    private final OutStream              out;
    private final ZscriptTokenExpression expression;

    public CommandContext(final Zscript zscript, final ContextView contextView, final OutStream out) {
        super(contextView);
        this.zscript = zscript;
        this.out = out;
        this.expression = new ZscriptTokenExpression(() -> contextView.getReader().iterator());
    }

    public ZscriptCommandOutStream getOutStream() {
        return out.asCommandOutStream();
    }

    @Override
    public OptionalInt getField(final byte key) {
        return expression.getField(key);
    }

    @Override
    public int getFieldCount() {
        return expression.getFieldCount();
    }

    @Override
    public boolean hasBigField() {
        return expression.hasBigField();
    }

    @Override
    public BlockIterator getBigField() {
        return expression.getBigField();
    }

    @Override
    public int getBigFieldSize() {
        return expression.getBigFieldSize();
    }

    @Override
    public byte[] getBigFieldData() {
        return expression.getBigFieldData();
    }

    @Override
    public boolean status(final byte status) {
        if (super.status(status)) {
            out.asCommandOutStream().writeField(Zchars.Z_STATUS, status);
            return true;
        }
        return false;
    }

    public OptIterator<ZscriptField> fieldIterator() {
        return new OptIterator<>() {
            final OptIterator<ReadToken> iter = expression.iteratorToMarker();

            @Override
            public Optional<ZscriptField> next() {
                return iter.next().map(ZscriptField::new);
            }
        };
    }

    public boolean verify() {
        final OptIterator<ReadToken> iter      = expression.iteratorToMarker();
        final BitSet                 foundCmds = new BitSet(26);
        for (Optional<ReadToken> opt = iter.next(); opt.isPresent(); opt = iter.next()) {
            final ReadToken rt  = opt.get();
            final byte      key = rt.getKey();
            if (Zchars.isNumericKey(key)) {
                if (foundCmds.get(key - 'A')) {
                    commandComplete();
                    status(ZscriptStatus.REPEATED_KEY);
                    return false;
                }
                foundCmds.set(key - 'A');
                if (rt.getDataSize() > 2) {
                    commandComplete();
                    status(ZscriptStatus.FIELD_TOO_LONG);
                    return false;
                }
            } else if (!Zchars.isBigField(key)) {
                commandComplete();
                status(ZscriptStatus.INVALID_KEY);
                return false;
            }
        }
        return true;
    }

    public void activate() {
        contextView.activate();
    }

    public Zscript getZscript() {
        return zscript;
    }

    public int getChannelIndex() {
        return contextView.getChannelIndex();
    }
}
