package net.zscript.javareceiver.execution;

import java.util.BitSet;
import java.util.Optional;
import java.util.OptionalInt;

import net.zscript.javareceiver.core.CommandOutStream;
import net.zscript.javareceiver.core.SequenceOutStream;
import net.zscript.javareceiver.core.Zscript;
import net.zscript.javareceiver.semanticParser.ContextView;
import net.zscript.model.components.Zchars;
import net.zscript.model.components.ZscriptStatus;
import net.zscript.tokenizer.TokenBuffer.TokenReader.ReadToken;
import net.zscript.tokenizer.ZscriptExpression;
import net.zscript.tokenizer.ZscriptField;
import net.zscript.tokenizer.ZscriptTokenExpression;
import net.zscript.tokenizer.ZscriptTokenField;
import net.zscript.util.BlockIterator;
import net.zscript.util.ByteString;
import net.zscript.util.OptIterator;

/**
 * This is the toolkit that is made available to an executing command, allowing access to the command's fields. It also exposes control of the status and command completion and
 * async execution flow.
 */
public class CommandContext extends AbstractContext implements ZscriptExpression {
    private final Zscript                zscript;
    private final SequenceOutStream      out;
    private final ZscriptTokenExpression expression;

    public CommandContext(final Zscript zscript, final ContextView contextView, final SequenceOutStream out) {
        super(contextView);
        this.zscript = zscript;
        this.out = out;
        this.expression = new ZscriptTokenExpression(() -> contextView.getReader().tokenIterator());
    }

    public CommandOutStream getOutStream() {
        return out.asCommandOutStream();
    }

    @Override
    public OptionalInt getField(final byte key) {
        return expression.getField(key);
    }

    @Override
    public Optional<? extends ZscriptField> getZscriptField(byte key) {
        return expression.getZscriptField(key);
    }

    @Override
    public int getFieldCount() {
        return expression.getFieldCount();
    }

    @Override
    public boolean hasBigField() {
        return expression.hasBigField();
    }

    /**
     * Produces a BlockIterator that iterates over multiple big-field items appearing in a single expression, including any extension tokens, thus providing access to all the
     * big-field bytes.
     *
     * @return fully big-field-aware data iterator
     */
    public BlockIterator bigFieldDataIterator() {
        return expression.bigFieldDataIterator();
    }

    @Override
    public int getBigFieldSize() {
        return expression.getBigFieldSize();
    }

    @Override
    public ByteString getBigFieldAsByteString() {
        return expression.getBigFieldAsByteString();
    }

    @Override
    public boolean status(final byte status) {
        if (super.status(status)) {
            out.asCommandOutStream().writeField(Zchars.Z_STATUS, status);
            return true;
        }
        return false;
    }

    /**
     * Returns  an iterator that supplies ZscriptField objects representing the fields (big-fields and numeric) in the current ZscriptExpression.
     * <p/>
     * Note: relies on {@link ZscriptTokenExpression#iteratorToMarker()} to supply tokens, which should skip extension tokens correctly.
     *
     * @return OptIterator for all the fields in this expression
     */
    public OptIterator<ZscriptField> fieldIterator() {
        return new OptIterator<>() {
            final OptIterator<ReadToken> iter = expression.iteratorToMarker();

            @Override
            public Optional<ZscriptField> next() {
                return iter.next().map(ZscriptTokenField::new);
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
