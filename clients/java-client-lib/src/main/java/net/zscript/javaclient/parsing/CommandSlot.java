package net.zscript.javaclient.parsing;

import static net.zscript.javaclient.parsing.CommandSlot.ResponseStatus.PARSE_ERROR;
import static net.zscript.javaclient.parsing.CommandSlot.ResponseStatus.TOO_BIG;
import static net.zscript.javaclient.parsing.CommandSlot.ResponseStatus.UNKNOWN_ERROR;
import static net.zscript.javaclient.parsing.Zchars.ADDRESS_PREFIX;
import static net.zscript.javaclient.parsing.Zchars.ANDTHEN_SYMBOL;
import static net.zscript.javaclient.parsing.Zchars.BIGFIELD_PREFIX_MARKER;
import static net.zscript.javaclient.parsing.Zchars.BIGFIELD_QUOTE_MARKER;
import static net.zscript.javaclient.parsing.Zchars.BROADCAST_PREFIX;
import static net.zscript.javaclient.parsing.Zchars.CMD_PARAM;
import static net.zscript.javaclient.parsing.Zchars.DEBUG_PREFIX;
import static net.zscript.javaclient.parsing.Zchars.EOL_SYMBOL;
import static net.zscript.javaclient.parsing.Zchars.ORELSE_SYMBOL;
import static net.zscript.javaclient.parsing.Zchars.PARALLEL_PREFIX;
import static net.zscript.javaclient.parsing.Zchars.STRING_ESCAPE_SYMBOL;
import static net.zscript.javaclient.parsing.Zchars.getHex;
import static net.zscript.javaclient.parsing.Zchars.isWhitespace;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class CommandSlot {
    private static Logger LOG = LoggerFactory.getLogger(CommandSlot.class);

    static class SlotStatus {
        boolean started;
        boolean needsMoveAlong;
        boolean complete;

        boolean isFirstCommand;
        boolean isBroadcast;
        boolean isParallel;
        boolean hasData;

        public void resetToCommand() {
            isFirstCommand = false;
            started = false;
            needsMoveAlong = false;
            complete = false;
        }

        public void resetToSequence() {
            isFirstCommand = true;
            started = false;
            needsMoveAlong = false;
            complete = false;
            isBroadcast = false;
            isParallel = false;
        }
    }

    static class ParsingState {
        boolean hasFailed;
        boolean isComment;
        boolean isAddressing;
        boolean isParsingParallel;
        boolean waitingOnRun;
        boolean isAtSeqBeginning;
        boolean sequenceDone;

        boolean isInString;
        boolean isEscaped;
        boolean isInBig;
        boolean isInField;

        private void clearAll() {
            hasFailed = false;
            isComment = false;
            isAddressing = false;
            isParsingParallel = false;
            waitingOnRun = false;
            isAtSeqBeginning = false;
            sequenceDone = false;

            isInString = false;
            isEscaped = false;
            isInBig = false;
            isInField = false;
        }

        public void resetToCommand() {
            clearAll();
        }

        public void resetToSequence() {
            clearAll();
            isAtSeqBeginning = true;
        }

        public void resetToFail() {
            clearAll();
            hasFailed = true;
            waitingOnRun = true;
        }
    }

    enum ResponseStatus {
        OK(0x0),
        PARSE_ERROR(1),
        TOO_BIG(1),
        UNKNOWN_ERROR(1),
        CMD_FAIL(0x10);

        static int MAX_SYSTEM_CODE = 0xf;

        int code;

        ResponseStatus(int code) {
            this.code = code;
        }

        static boolean isSystemError(int code) {
            return code > MAX_SYSTEM_CODE;
        }

        boolean isSystemError() {
            return isSystemError(code);
        }

        boolean isUserError() {
            return !isSystemError(code);
        }
    }

    private CommandChannel channel;
    private FieldMap       fieldMap;
    private BigField       bigField;
    private SlotStatus     runStatus;
    private ParsingState   state;
    private String         failString;

//  ZcodeLockSet<ZP>                    lockSet;
    private ResponseStatus respStatus;

    private byte terminator;
    private byte starter;

    CommandSlot(BigField bigField, CommandChannel channel) {
        this.bigField = bigField;
        this.channel = channel;

        this.terminator = Zchars.EOL_SYMBOL.ch;
        this.starter = Zchars.EOL_SYMBOL.ch;
    }

    boolean acceptByte(byte c) { // returns false <=> the data couldn't be used and needs to be resent later.
        boolean wasWaiting = state.waitingOnRun;
        runStatus.hasData = true;
        if (state.hasFailed || state.isComment || state.isAddressing || state.isParsingParallel || state.waitingOnRun || state.isAtSeqBeginning
                || state.sequenceDone) {
            acceptEdgeCase(c);
        } else {
            if (state.isInString) {
                inStringParse(c);
            } else if (isWhitespace(c)) {
                // drops out.
            } else if (state.isInBig) {
                inBigParse(c);
            } else if (state.isInField) {
                inFieldParse(c);
            } else {
                outerCommandParse(c);
            }
        }
        return !wasWaiting; // waiting on run is the only circumstance where we don't want data.
    }

    void finish() {
        state.waitingOnRun = false;
        if (!state.hasFailed) {
            if (EOL_SYMBOL.matches(terminator)) {
                resetToSequence();
            } else if (ANDTHEN_SYMBOL.matches(terminator)) {
                resetToCommand();
            } else if (ORELSE_SYMBOL.matches(terminator)) {
                state.sequenceDone = true;
            }
            starter = terminator;
        } else {
            if (EOL_SYMBOL.matches(terminator)) {
                resetToSequence();
                starter = terminator;
            } else if (ORELSE_SYMBOL.matches(terminator)) {
                if (respStatus.isUserError()) {
                    resetToCommand();
                    starter = terminator;
                }
            }
        }
    }

    void clearHasData() {
        runStatus.hasData = false;
    }

    CommandChannel getCommandChannel() {
        return channel;
    }

    public FieldMap getFieldMap() {
        return fieldMap;
    }

    BigField getBigField() {
        return bigField;
    }

    void resetToCommand() {
        fieldMap.reset();
        bigField.reset();
        state.resetToCommand();
        runStatus.resetToCommand();
    }

    void resetToSequence() {
        fieldMap.reset();
        bigField.reset();
//        lockSet.setAll();
        state.resetToSequence();
        runStatus.resetToSequence();
        respStatus = ResponseStatus.OK;
        failString = null;
    }

    void parseFail(byte c, ResponseStatus status, String message) {
        state.resetToFail();
        respStatus = status;
        failString = message;

        terminator = c;
    }

    void failExternal(ResponseStatus status) {
        state.resetToFail();
        state.waitingOnRun = false;
        respStatus = status;
        failString = null;
    }

    void inStringParse(byte c) {
        if (BIGFIELD_QUOTE_MARKER.matches(c)) {
            state.isInString = false;
            if (state.isEscaped || bigField.isInNibble()) {
                parseFail(c, PARSE_ERROR, Strings.failParseBadStringEnd);
                return;
            }
            return;
        }
        if (EOL_SYMBOL.matches(c)) {
            parseFail(c, PARSE_ERROR, Strings.failParseNoStringEnd);
            return;
        }
        if (bigField.isAtEnd()) {
            parseFail(c, TOO_BIG, Strings.failParseStringTooLong);
            return;
        }
        if (state.isEscaped) {
            byte val = getHex(c);
            if (val == -1) {
                parseFail(c, PARSE_ERROR, Strings.failParseStringEscaping);
                return;
            }
            if (bigField.isInNibble()) {
                state.isEscaped = false;
            }
            bigField.addNibble(val);
        } else if (STRING_ESCAPE_SYMBOL.matches(c)) {
            state.isEscaped = true;
        } else {
            bigField.addByte(c);
        }
    }

    void inBigParse(byte c) {
        byte val = getHex(c);
        if (val == -1) {
            state.isInBig = false;
            if (bigField.isInNibble()) {
                parseFail(c, PARSE_ERROR, Strings.failParseBigOdd);
                return;
            }
            outerCommandParse(c);
            return;
        }
        if (!bigField.addNibble(val)) {
            parseFail(c, TOO_BIG, Strings.failParseBigTooLong);
            return;
        }
    }

    void inFieldParse(byte c) {
        byte val = Zchars.getHex(c);
        if (val == -1) {
            state.isInField = false;
            outerCommandParse(c);
            return;
        }
        if (!fieldMap.add4(val)) {
            parseFail(c, TOO_BIG, Strings.failParseFieldTooLong);
            return;
        }
    }

    void outerCommandParse(byte c) {
        if (Zchars.isParamKey(c)) {
            state.isInField = true;
            if (!fieldMap.addBlank(c)) {
                parseFail(c, PARSE_ERROR, Strings.failParseFieldRepeated);
            }
        } else if (BIGFIELD_PREFIX_MARKER.matches(c)) {
            state.isInBig = true;
        } else if (BIGFIELD_QUOTE_MARKER.matches(c)) {
            state.isInString = true;
        } else if (Zchars.isCommandEnd(c)) {
            state.waitingOnRun = true;
            terminator = c;
        } else {
            parseFail(c, PARSE_ERROR, Strings.failParseUnknownChar);
        }
    }

    void lockingParse(byte c) {
        byte val = Zchars.getHex(c);
        if (val == -1) {
            state.isParsingParallel = false;
            acceptByte(c);
            return;
        }
//        if (!lockSet.setLocks4(val)) {
//            parseFail(c, TOO_BIG, Strings.failParseTooManyLocks);
//        }
    }

    void sequenceBeginingParse(byte c) {
        if (BROADCAST_PREFIX.matches(c)) {
            if (runStatus.isBroadcast) {
                parseFail(c, PARSE_ERROR, Strings.failParseBroadcastMultiple);
            }
            runStatus.isBroadcast = true;
        } else if (DEBUG_PREFIX.matches(c)) {
            state.isComment = true;
        } else if (ADDRESS_PREFIX.matches(c)) {
            state.isAtSeqBeginning = false;
            state.isAddressing = true;
            fieldMap.add16(CMD_PARAM.ch, 0x11);
        } else if (PARALLEL_PREFIX.matches(c)) {
            if (runStatus.isParallel) {
                parseFail(c, PARSE_ERROR, Strings.failParseLocksMultiple);
            } else {
//                lockSet.clearAll();
                state.isParsingParallel = true;
                runStatus.isParallel = true;
            }
        } else {
            state.isAtSeqBeginning = false;
            acceptByte(c);
        }
    }

    void acceptEdgeCase(byte c) {
        if (state.waitingOnRun) {
            // left blank, just drops out.
        } else if (state.sequenceDone || state.isComment || (state.hasFailed && (!ORELSE_SYMBOL.matches(c) || respStatus.isSystemError()))) {
            if (EOL_SYMBOL.matches(c)) {
                resetToSequence();
                terminator = EOL_SYMBOL.ch;
                starter = terminator;
            }
        } else if (state.hasFailed && ORELSE_SYMBOL.matches(c) && respStatus.isSystemError()) {
            resetToCommand();
            starter = ORELSE_SYMBOL.ch;
        } else if (state.isParsingParallel) {
            if (!isWhitespace(c)) {
                lockingParse(c);
            }
        } else if (state.isAtSeqBeginning) {
            if (!isWhitespace(c)) {
                sequenceBeginingParse(c);
            }
        } else if (state.isAddressing) {
            bigField.addByte(c);
            if (EOL_SYMBOL.matches(c)) {
                state.waitingOnRun = true;
                terminator = c;
            }
        } else {
            parseFail(c, UNKNOWN_ERROR, Strings.failParseOther);
        }
    }

}
