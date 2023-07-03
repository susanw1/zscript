package org.zcode.javareceiver.execution;

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

    public OptIterator<ReadToken> iterator() {
        return new OptIterator<ZcodeTokenBuffer.TokenReader.ReadToken>() {
            OptIterator<ReadToken> internal = parser.getReader().iterator();

            @Override
            public Optional<ReadToken> next() {
                return internal.next().filter(t -> !t.isMarker());
            }

        };
    }

    public void execute() {
        // TODO: implement
    }

    public void moveAlong() {
        // TODO: write
    }

    public boolean verify() {
        OptIterator<ReadToken> iter = iterator();
        for (Optional<ReadToken> opt = iter.next(); opt.isPresent(); opt = iter.next()) {
            ReadToken rt  = opt.get();
            byte      key = rt.getKey();
            if ('A' <= key && key <= 'Z') {
                if (rt.getDataSize() > 2) {
                    fail();
                    return false;
                }
            } else if (key != '"' && key != '+') {
                fail();
                return false;
            }
        }
        return true;
    }

    private void fail() {
        if (!out.isOpen()) {
            out.open();
        }
        out.writeStatus(ZcodeStatus.TOKEN_FORMAT_ERROR);
        out.endSequence();
        out.close();
        parser.fail();
    }
}
