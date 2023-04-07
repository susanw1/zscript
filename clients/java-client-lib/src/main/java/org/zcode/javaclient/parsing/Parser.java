package org.zcode.javaclient.parsing;

import static org.zcode.javaclient.parsing.Zchars.ADDRESS_PREFIX;
import static org.zcode.javaclient.parsing.Zchars.ANDTHEN_SYMBOL;
import static org.zcode.javaclient.parsing.Zchars.BIGFIELD_PREFIX_MARKER;
import static org.zcode.javaclient.parsing.Zchars.BIGFIELD_QUOTE_MARKER;
import static org.zcode.javaclient.parsing.Zchars.CMD_PARAM;
import static org.zcode.javaclient.parsing.Zchars.DEBUG_PREFIX;
import static org.zcode.javaclient.parsing.Zchars.EOL_SYMBOL;
import static org.zcode.javaclient.parsing.Zchars.ORELSE_SYMBOL;
import static org.zcode.javaclient.parsing.Zchars.PARALLEL_PREFIX;
import static org.zcode.javaclient.parsing.Zchars.STRING_ESCAPE_SYMBOL;
import static org.zcode.javaclient.parsing.Zchars.getHex;
import static org.zcode.javaclient.parsing.Zchars.isWhitespace;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.zcode.javaclient.parsing.Address.AddressBuilder;
import org.zcode.javaclient.parsing.BigField.BigFieldBuilder;
import org.zcode.javaclient.parsing.Comment.CommentBuilder;
import org.zcode.javaclient.parsing.Params.ParamsBuilder;

public class Parser {
    private static final Logger LOG = LoggerFactory.getLogger(Parser.class);

    private final boolean responseParser;

    // Sequence-based state
    private final AddressBuilder addressBuilder;
    private final CommentBuilder commentBuilder;

    // Command-based state (to be reset once each Command/Response is created)
    private final ParamsBuilder   fieldMap;
    private final BigFieldBuilder bigField;

    private final ParsingState state;
//    private final SlotStatus   runStatus;
//    private String failString;

    private ResponseStatus respStatus;

    private byte terminator;
    private byte starter;

    public Parser(boolean responseParser) {
        this.responseParser = responseParser;

        this.fieldMap = Params.builder();
        this.bigField = BigField.builder();
        this.addressBuilder = Address.builder();
        this.commentBuilder = Comment.builder();

        this.state = new ParsingState();

        this.terminator = Zchars.EOL_SYMBOL.ch;
        this.starter = Zchars.EOL_SYMBOL.ch;
    }

    /**
     * Accepts the next byte of a zcode command stream. Textual data (ie big-field data) is assumed to be in UTF-8 format, and is accepted byte-by-byte.
     * 
     * @param c the byte (character) to accept.
     * @return true if the parser accepted the byte, and false if the parser has a complete command/response to be collected.
     */
    public boolean acceptByte(byte c) {
        boolean wasWaiting = state.waitingOnRun;
//        runStatus.hasData = true;
        if (state.waitingOnRun) {
            // just drops out.
        } else if (state.hasFailed || state.isComment || state.isAddressing || state.isParsingParallel || state.isAtSeqBeginning || state.sequenceDone) {
            acceptEdgeCase(c);
        } else if (state.isInString) {
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

        return !wasWaiting; // waiting on run is the only circumstance where we don't want data.
    }

    public void finish() {
        state.waitingOnRun = false;
//        if (!state.hasFailed) {
        if (EOL_SYMBOL.matches(terminator)) {
            resetToSequence();
        } else if (ANDTHEN_SYMBOL.matches(terminator)) {
            resetToCommand();
        } else if (ORELSE_SYMBOL.matches(terminator)) {
            state.sequenceDone = true;
        }
        starter = terminator;
//        } else {
//            if (EOL_SYMBOL.matches(terminator)) {
//                resetToSequence();
//                starter = terminator;
//            } else if (ORELSE_SYMBOL.matches(terminator)) {
//                if (respStatus.isUserError()) {
//                    resetToCommand();
//                    starter = terminator;
//                }
//            }
//        }
    }

//    private void clearHasData() {
//        runStatus.hasData = false;
//    }

    private void resetToCommand() {
        fieldMap.reset();
        bigField.reset();
        state.resetToCommand();
//        runStatus.resetToCommand();
    }

    private void resetToSequence() {
        fieldMap.reset();
        bigField.reset();
//        lockSet.setAll();
        state.resetToSequence();
//        runStatus.resetToSequence();
//        respStatus = ResponseStatus.OK;
//        failString = null;
    }

    private void parseFail(byte c, ResponseStatus status, String message) {
        state.resetToFail();
        respStatus = status;
//        failString = message;

        terminator = c;
    }

//    private void failExternal(ResponseStatus status) {
//        state.resetToFail();
//        state.waitingOnRun = false;
//        respStatus = status;
//        failString = null;
//    }

    private void inStringParse(byte c) {
        if (BIGFIELD_QUOTE_MARKER.matches(c)) {
            state.isInString = false;
            if (state.isEscaped || bigField.isInNibble()) {
                parseFail(c, ResponseStatus.PARSE_ERROR, Strings.failParseBadStringEnd);
                return;
            }
            return;
        }
        if (EOL_SYMBOL.matches(c)) {
            parseFail(c, ResponseStatus.PARSE_ERROR, Strings.failParseNoStringEnd);
            return;
        }
//        // Do this in the validation stage
//        if (bigField.isAtEnd()) {
//            parseFail(c, ResponseStatus.TOO_BIG, Strings.failParseStringTooLong);
//            return;
//        }
        if (state.isEscaped) {
            byte val = getHex(c);
            if (val == -1) {
                parseFail(c, ResponseStatus.PARSE_ERROR, Strings.failParseStringEscaping);
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

    private void inBigParse(byte c) {
        byte val = getHex(c);
        if (val == -1) {
            state.isInBig = false;
            if (bigField.isInNibble()) {
                parseFail(c, ResponseStatus.PARSE_ERROR, Strings.failParseBigOdd);
                return;
            }
            outerCommandParse(c);
            return;
        }
        if (!bigField.addNibble(val)) {
            parseFail(c, ResponseStatus.TOO_BIG, Strings.failParseBigTooLong);
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
            parseFail(c, ResponseStatus.TOO_BIG, Strings.failParseFieldTooLong);
            return;
        }
    }

    private void outerCommandParse(byte c) {
        if (Zchars.isParamKey(c)) {
            state.isInField = true;
            if (!fieldMap.addBlank(c)) {
                parseFail(c, ResponseStatus.PARSE_ERROR, Strings.failParseFieldRepeated);
            }
        } else if (BIGFIELD_PREFIX_MARKER.matches(c)) {
            state.isInBig = true;
        } else if (BIGFIELD_QUOTE_MARKER.matches(c)) {
            state.isInString = true;
        } else if (Zchars.isCommandEnd(c)) {
            state.waitingOnRun = true;
            terminator = c;
        } else {
            parseFail(c, ResponseStatus.PARSE_ERROR, Strings.failParseUnknownChar);
        }
    }

    private void lockingParse(byte c) {
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

    private void sequenceBeginningParse(byte c) {
        if (DEBUG_PREFIX.matches(c)) {
            state.isComment = true;
        } else if (ADDRESS_PREFIX.matches(c)) {
            state.isAtSeqBeginning = false;
            state.isAddressing = true;
            fieldMap.add16(CMD_PARAM.ch, 0x11);
        } else if (PARALLEL_PREFIX.matches(c)) {
//            if (runStatus.isParallel) {
//                parseFail(c, ResponseStatus.PARSE_ERROR, Strings.failParseLocksMultiple);
//            } else {
//                lockSet.clearAll();
            state.isParsingParallel = true;
//                runStatus.isParallel = true;
//            }
        } else {
            state.isAtSeqBeginning = false;
            acceptByte(c);
        }
    }

    private void acceptEdgeCase(byte c) {
        if (state.sequenceDone || state.isComment || (state.hasFailed && (!ORELSE_SYMBOL.matches(c) || respStatus.isSystemError()))) {
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
                sequenceBeginningParse(c);
            }
        } else if (state.isAddressing) {
            bigField.addByte(c);
            if (EOL_SYMBOL.matches(c)) {
                state.waitingOnRun = true;
                terminator = c;
            }
        } else {
            parseFail(c, ResponseStatus.UNKNOWN_ERROR, Strings.failParseOther);
        }
    }

//    private static class SlotStatus {
//        boolean started;
//        boolean needsMoveAlong;
//        boolean complete;
//
//        boolean isFirstCommand;
//        boolean isParallel;
//        boolean hasData;
//
//        public void resetToCommand() {
//            isFirstCommand = false;
//            started = false;
//            needsMoveAlong = false;
//            complete = false;
//        }
//
//        public void resetToSequence() {
//            isFirstCommand = true;
//            started = false;
//            needsMoveAlong = false;
//            complete = false;
//            isParallel = false;
//        }
//    }

    /**
     * State of the parser itself, to keep track of what to do with the next character.
     */
    private static class ParsingState {
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
    }

    private enum ResponseStatus {
        OK(0x0),
        PARSE_ERROR(3),
        TOO_BIG(8),
        UNKNOWN_ERROR(2),
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
}
