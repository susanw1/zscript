package net.zscript.javareceiver.execution;

import javax.annotation.Nonnull;
import java.util.BitSet;
import java.util.Optional;
import java.util.OptionalInt;
import java.util.stream.Stream;

import net.zscript.javareceiver.core.CommandOutStream;
import net.zscript.javareceiver.core.SequenceOutStream;
import net.zscript.javareceiver.core.Zscript;
import net.zscript.javareceiver.semanticparser.ContextView;
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

    @Nonnull
    public CommandOutStream getOutStream() {
        return out.asCommandOutStream();
    }

    @Nonnull
    @Override
    public Stream<? extends ZscriptField> fields() {
        return expression.fields();
    }

    @Nonnull
    @Override
    public OptionalInt getField(final byte key) {
        return expression.getField(key);
    }

    @Nonnull
    @Override
    public Optional<? extends ZscriptField> getZscriptField(byte key) {
        return expression.getZscriptField(key);
    }

    @Override
    public int getFieldCount() {
        return expression.getFieldCount();
    }

    @Override
    @Deprecated
    public boolean hasBigField() {
        return expression.hasBigField();
    }

    /**
     * Produces a BlockIterator that iterates over multiple big-field items appearing in a single expression, including any extension tokens, thus providing access to all the
     * big-field bytes.
     *
     * @return fully big-field-aware data iterator
     */
    @Nonnull
    public Optional<BlockIterator> dataIterator(byte c) {
        return expression.getFieldData(c);
    }

    @Override
    @Deprecated
    public int getBigFieldSize() {
        return expression.getBigFieldSize();
    }

    @Nonnull
    @Override
    @Deprecated
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
     * Returns  an iterator that supplies ZscriptField objects representing all the fields in the current ZscriptExpression.
     * <p/>
     * Note: relies on {@link ZscriptTokenExpression#iteratorToMarker()} to supply tokens, which should skip extension tokens correctly (ie not return them, but count them as part
     * of their precursor token).
     *
     * @return OptIterator for all the fields in this expression
     */
    @Nonnull
    public OptIterator<ZscriptField> fieldIterator() {
        return new OptIterator<>() {
            final OptIterator<ReadToken> iter = expression.iteratorToMarker();

            @Nonnull
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
            if (Zchars.isExpressionKey(key)) {
                if (foundCmds.get(key - 'A')) {
                    commandComplete();
                    status(ZscriptStatus.REPEATED_KEY);
                    return false;
                }
                foundCmds.set(key - 'A');
                // // Obsolete: fields are no longer size limited like this
                //                if (rt.getDataSize() > 2) {
                //                    commandComplete();
                //                    status(ZscriptStatus.FIELD_TOO_LONG);
                //                    return false;
                //                }
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

    @Nonnull
    public Zscript getZscript() {
        return zscript;
    }

    public int getChannelIndex() {
        return contextView.getChannelIndex();
    }

    @Override
    public String toString() {
        return "CommandContext{" + "expression=" + expression + '}';
    }
}
