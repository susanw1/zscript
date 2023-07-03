package org.zcode.javareceiver.execution;

import java.util.BitSet;
import java.util.Optional;

import org.zcode.javareceiver.core.ZcodeOutStream;
import org.zcode.javareceiver.core.ZcodeStatus;
import org.zcode.javareceiver.semanticParser.SemanticParser;
import org.zcode.javareceiver.tokenizer.OptIterator;
import org.zcode.javareceiver.tokenizer.ZcodeTokenBuffer;
import org.zcode.javareceiver.tokenizer.ZcodeTokenBuffer.TokenReader.ReadToken;

public class ZcodeCommandView {
    private final SemanticParser parser;
    private final ZcodeOutStream out;

    public ZcodeCommandView(SemanticParser parser, ZcodeOutStream out) {
        this.parser = parser;
        this.out = out;
    }

    public ZcodeOutStream getOutStream() {
        return out;
    }

    public OptIterator<ReadToken> iterator() {
        return new OptIterator<ZcodeTokenBuffer.TokenReader.ReadToken>() {
            OptIterator<ReadToken> internal = parser.getReader().iterator();

            @Override
            public Optional<ReadToken> next() {
                return internal.next().filter(t -> !t.isMarker());
            }

        };
    }

    public boolean verify() {
        OptIterator<ReadToken> iter      = iterator();
        BitSet                 foundCmds = new BitSet(26);
        for (Optional<ReadToken> opt = iter.next(); opt.isPresent(); opt = iter.next()) {
            ReadToken rt  = opt.get();
            byte      key = rt.getKey();
            if ('A' <= key && key <= 'Z') {
                if (foundCmds.get(key - 'A')) {
                    setComplete();
                    error(ZcodeStatus.TOKEN_ERROR);
                    return false;
                }
                foundCmds.set(key - 'A');
                if (rt.getDataSize() > 2) {
                    setComplete();
                    error(ZcodeStatus.TOKEN_ERROR);
                    return false;
                }
            } else if (key != '"' && key != '+') {
                setComplete();
                error(ZcodeStatus.TOKEN_ERROR);
                return false;
            }
        }
        return true;
    }

    private void error(byte status) {
        if (!out.isOpen()) {
            out.open();
        }
        out.writeField('S', status);
        out.endSequence();
        out.close();
        parser.error();
    }

    public void fail(byte status) {
        if (status < 0x10) {
            parser.error();
        } else {
            parser.softFail();
        }
        out.writeField('S', status);
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

    public boolean isEmpty() {
        return parser.getReader().getFirstReadToken().isMarker();
    }

    public void activate() {
        parser.activate();
    }

    public boolean isComplete() {
        return parser.isComplete();
    }

}
