package net.zscript.javareceiver.treeParser;

import static net.zscript.javareceiver.tokenizer.ZcodeTokenBuffer.isSequenceEndMarker;

import java.util.ArrayList;
import java.util.List;
import java.util.Optional;

import net.zscript.javareceiver.tokenizer.OptIterator;
import net.zscript.javareceiver.tokenizer.ZcodeTokenizer;
import net.zscript.javareceiver.tokenizer.ZcodeTokenBuffer.TokenReader.ReadToken;

public class ZcodeTreeParser {

    public static ZcodeSequenceUnitPlace parseAll(ReadToken start) {
        int lowest     = 0;
        int parenCount = 0;

        OptIterator<ReadToken> it = start.getNextTokens();
        for (Optional<ReadToken> opt = it.next(); opt.isPresent() && !opt.get().isSequenceEndMarker(); opt = it.next()) {
            ReadToken t = opt.get();
            if (t.getKey() == ZcodeTokenizer.CMD_END_OPEN_PAREN) {
                parenCount++;
            } else if (t.getKey() == ZcodeTokenizer.CMD_END_CLOSE_PAREN) {
                parenCount--;
            }
            if (parenCount < lowest) {
                lowest = parenCount;
            }
        }
        ZcodeSequenceUnitPlace place = new ZcodeSequenceUnitPlace(start, ZcodeTokenizer.NORMAL_SEQUENCE_END, -lowest);
        while (place.parse()) {
            ;
        }
        return place;
    }

    static ZcodeSequenceUnit parse(ReadToken start, byte endMarker, int parenStartCount) {
        int     parenLevel    = parenStartCount;
        boolean hasMarker     = false;
        int     minParenLevel = 0x10000;

        boolean hadNonParen = false;

        OptIterator<ReadToken> it = start.getNextTokens();
        for (Optional<ReadToken> opt = it.next(); opt.isPresent()
                && (parenLevel != 0 || opt.get().getKey() != endMarker) && !opt.get().isSequenceEndMarker(); opt = it.next()) {
            ReadToken t = opt.get();
            hasMarker |= t.isMarker();
            if (hadNonParen) {
                if (t.getKey() != ZcodeTokenizer.CMD_END_CLOSE_PAREN) {
                    minParenLevel = Math.min(minParenLevel, parenLevel);
                }
            } else {
                if (t.getKey() != ZcodeTokenizer.CMD_END_OPEN_PAREN) {
                    hadNonParen = true;
                }
            }
            if (t.getKey() == ZcodeTokenizer.CMD_END_OPEN_PAREN) {
                parenLevel++;
            } else if (t.getKey() == ZcodeTokenizer.CMD_END_CLOSE_PAREN) {
                parenLevel--;
            }
        }
        if (minParenLevel == 0x10000) {
            minParenLevel = 0;
        }
        if (!hasMarker) {
//            System.out.println("Command: " + (char) start.getKey() + " : " + Integer.toHexString(Byte.toUnsignedInt(endMarker)));
            return new ZcodeCommandUnit(start);
        }

        ReadToken firstOnLevel = null;
        it = start.getNextTokens();
        parenLevel = parenStartCount;
        for (Optional<ReadToken> opt = it.next(); opt.isPresent()
                && (parenLevel != 0 || opt.get().getKey() != endMarker) && !isSequenceEndMarker(opt.get().getKey()); opt = it.next()) {
            ReadToken t = opt.get();
            if (parenLevel == minParenLevel) {
                if (firstOnLevel == null) {
                    firstOnLevel = t;
                }
                if (t.getKey() == ZcodeTokenizer.CMD_END_ORELSE) {
//                    System.out.println("Or: " + (char) start.getKey() + " : " + Integer.toHexString(Byte.toUnsignedInt(endMarker)));
                    return new ZcodeOrSeperatedUnit(firstOnLevel, Math.max(parenStartCount - minParenLevel, 0), it.next().get(), endMarker);
                }
            }
            if (t.getKey() == ZcodeTokenizer.CMD_END_OPEN_PAREN) {
                parenLevel++;
            } else if (t.getKey() == ZcodeTokenizer.CMD_END_CLOSE_PAREN) {
                parenLevel--;
            }
        }

        List<ZcodeSequenceUnitPlace> units = new ArrayList<>();

        int parenLeft = Math.max(parenStartCount - minParenLevel, 0);

        ReadToken prevStart   = null;
        boolean   hasTokens   = false;
        boolean   lastWasOpen = false;
        parenLevel = parenStartCount;
        it = start.getNextTokens();
        Optional<ReadToken> opt;
        for (opt = it.next(); opt.isPresent()
                && (parenLevel != 0 && !isSequenceEndMarker(opt.get().getKey()) || opt.get().getKey() != endMarker); opt = it.next()) {
            ReadToken t = opt.get();
            if (prevStart == null && hasTokens) {
                prevStart = t;
            }
            if (parenLevel == minParenLevel || lastWasOpen && parenLevel == minParenLevel + 1 && t.getKey() == ZcodeTokenizer.CMD_END_CLOSE_PAREN) {
                if (prevStart == null) {
                    prevStart = t;
                }
                if (t.isMarker()) {
                    units.add(new ZcodeSequenceUnitPlace(prevStart, t.getKey(), parenLeft));
                    lastWasOpen = t.getKey() == ZcodeTokenizer.CMD_END_OPEN_PAREN;
                    parenLeft = 0;
                    prevStart = null;
                    hasTokens = true;
                }
            }
            if (t.getKey() == ZcodeTokenizer.CMD_END_OPEN_PAREN) {
                parenLevel++;
            } else if (t.getKey() == ZcodeTokenizer.CMD_END_CLOSE_PAREN) {
                parenLevel--;
            }
        }
        if (!hasTokens) {
            it = start.getNextTokens();
            for (opt = it.next(); opt.isPresent() && !opt.get().isMarker(); opt = it.next()) {
            }
//            System.out.println("Command2: " + (char) start.getKey() + " : " + Integer.toHexString(Byte.toUnsignedInt(endMarker)));
            return new ZcodeCommandUnit(opt.get());
        }
        if (prevStart == null) {
            prevStart = opt.get();
        }
        units.add(new ZcodeSequenceUnitPlace(prevStart, endMarker, parenLeft));

//            System.out.println("And: " + (char) start.getKey() + " : " + Integer.toHexString(Byte.toUnsignedInt(endMarker)));
        return new ZcodeAndSequenceUnit(units);
    }

}
