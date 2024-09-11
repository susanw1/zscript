package net.zscript.javareceiver.treeParser;

import java.util.ArrayList;
import java.util.List;
import java.util.Optional;

import static net.zscript.tokenizer.TokenBuffer.isSequenceEndMarker;

import net.zscript.tokenizer.TokenBuffer.TokenReader.ReadToken;
import net.zscript.tokenizer.Tokenizer;
import net.zscript.util.OptIterator;

public class ZscriptTreeParser {

    public static ZscriptSequenceUnitPlace parseAll(ReadToken start) {
        int lowest     = 0;
        int parenCount = 0;

        OptIterator<ReadToken> it = start.tokenIterator();
        for (Optional<ReadToken> opt = it.next(); opt.isPresent() && !opt.get().isSequenceEndMarker(); opt = it.next()) {
            ReadToken t = opt.get();
            if (t.getKey() == Tokenizer.CMD_END_OPEN_PAREN) {
                parenCount++;
            } else if (t.getKey() == Tokenizer.CMD_END_CLOSE_PAREN) {
                parenCount--;
            }
            if (parenCount < lowest) {
                lowest = parenCount;
            }
        }
        ZscriptSequenceUnitPlace place = new ZscriptSequenceUnitPlace(start, Tokenizer.NORMAL_SEQUENCE_END, -lowest);
        while (place.parse()) {
            // just keep parsing
        }
        return place;
    }

    static ZscriptSequenceUnit parse(ReadToken start, byte endMarker, int parenStartCount) {
        int     parenLevel    = parenStartCount;
        boolean hasMarker     = false;
        int     minParenLevel = 0x10000;

        boolean hadNonParen = false;

        OptIterator<ReadToken> it = start.tokenIterator();
        for (Optional<ReadToken> opt = it.next(); opt.isPresent()
                && (parenLevel != 0 || opt.get().getKey() != endMarker) && !opt.get().isSequenceEndMarker(); opt = it.next()) {
            ReadToken t = opt.get();
            hasMarker |= t.isMarker();
            if (hadNonParen) {
                if (t.getKey() != Tokenizer.CMD_END_CLOSE_PAREN) {
                    minParenLevel = Math.min(minParenLevel, parenLevel);
                }
            } else {
                if (t.getKey() != Tokenizer.CMD_END_OPEN_PAREN) {
                    hadNonParen = true;
                }
            }
            if (t.getKey() == Tokenizer.CMD_END_OPEN_PAREN) {
                parenLevel++;
            } else if (t.getKey() == Tokenizer.CMD_END_CLOSE_PAREN) {
                parenLevel--;
            }
        }
        if (minParenLevel == 0x10000) {
            minParenLevel = 0;
        }
        if (!hasMarker) {
            //            System.out.println("Command: " + (char) start.getKey() + " : " + Integer.toHexString(Byte.toUnsignedInt(endMarker)));
            return new ZscriptCommandUnit(start);
        }

        ReadToken firstOnLevel = null;
        it = start.tokenIterator();
        parenLevel = parenStartCount;
        for (Optional<ReadToken> opt = it.next(); opt.isPresent()
                && (parenLevel != 0 || opt.get().getKey() != endMarker) && !isSequenceEndMarker(opt.get().getKey()); opt = it.next()) {
            ReadToken t = opt.get();
            if (parenLevel == minParenLevel) {
                if (firstOnLevel == null) {
                    firstOnLevel = t;
                }
                if (t.getKey() == Tokenizer.CMD_END_ORELSE) {
                    //                    System.out.println("Or: " + (char) start.getKey() + " : " + Integer.toHexString(Byte.toUnsignedInt(endMarker)));
                    return new ZscriptOrSeparatedUnit(firstOnLevel, Math.max(parenStartCount - minParenLevel, 0), it.next().get(), endMarker);
                }
            }
            if (t.getKey() == Tokenizer.CMD_END_OPEN_PAREN) {
                parenLevel++;
            } else if (t.getKey() == Tokenizer.CMD_END_CLOSE_PAREN) {
                parenLevel--;
            }
        }

        List<ZscriptSequenceUnitPlace> units = new ArrayList<>();

        int parenLeft = Math.max(parenStartCount - minParenLevel, 0);

        ReadToken prevStart   = null;
        boolean   hasTokens   = false;
        boolean   lastWasOpen = false;
        parenLevel = parenStartCount;
        it = start.tokenIterator();
        Optional<ReadToken> opt;
        for (opt = it.next(); opt.isPresent()
                && (parenLevel != 0 && !isSequenceEndMarker(opt.get().getKey()) || opt.get().getKey() != endMarker); opt = it.next()) {
            ReadToken t = opt.get();
            if (prevStart == null && hasTokens) {
                prevStart = t;
            }
            if (parenLevel == minParenLevel || lastWasOpen && parenLevel == minParenLevel + 1 && t.getKey() == Tokenizer.CMD_END_CLOSE_PAREN) {
                if (prevStart == null) {
                    prevStart = t;
                }
                if (t.isMarker()) {
                    units.add(new ZscriptSequenceUnitPlace(prevStart, t.getKey(), parenLeft));
                    lastWasOpen = t.getKey() == Tokenizer.CMD_END_OPEN_PAREN;
                    parenLeft = 0;
                    prevStart = null;
                    hasTokens = true;
                }
            }
            if (t.getKey() == Tokenizer.CMD_END_OPEN_PAREN) {
                parenLevel++;
            } else if (t.getKey() == Tokenizer.CMD_END_CLOSE_PAREN) {
                parenLevel--;
            }
        }
        if (!hasTokens) {
            it = start.tokenIterator();
            for (opt = it.next(); opt.isPresent() && !opt.get().isMarker(); opt = it.next()) {
            }
            //            System.out.println("Command2: " + (char) start.getKey() + " : " + Integer.toHexString(Byte.toUnsignedInt(endMarker)));
            return new ZscriptCommandUnit(opt.get());
        }
        if (prevStart == null) {
            prevStart = opt.get();
        }
        units.add(new ZscriptSequenceUnitPlace(prevStart, endMarker, parenLeft));

        //            System.out.println("And: " + (char) start.getKey() + " : " + Integer.toHexString(Byte.toUnsignedInt(endMarker)));
        return new ZscriptAndSequenceUnit(units);
    }

}
