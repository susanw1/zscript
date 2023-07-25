package org.zcode.javareceiver.semanticParser;

import java.util.Optional;
import java.util.function.Supplier;

import org.zcode.javareceiver.core.Zcode;
import org.zcode.javareceiver.core.ZcodeLockSet;
import org.zcode.javareceiver.core.ZcodeLocks;
import org.zcode.javareceiver.core.ZcodeStatus;
import org.zcode.javareceiver.semanticParser.ZcodeAction.ActionType;
import org.zcode.javareceiver.tokenizer.TokenBufferIterator;
import org.zcode.javareceiver.tokenizer.Zchars;
import org.zcode.javareceiver.tokenizer.ZcodeTokenBuffer;
import org.zcode.javareceiver.tokenizer.ZcodeTokenBuffer.TokenReader;
import org.zcode.javareceiver.tokenizer.ZcodeTokenBuffer.TokenReader.ReadToken;
import org.zcode.javareceiver.tokenizer.ZcodeTokenBufferFlags;
import org.zcode.javareceiver.tokenizer.ZcodeTokenizer;

public class SemanticParser implements ParseState, ContextView {
    // 16 booleans, 5 uint8_t, 1 uint16_t = 9 bytes of status

    public static final byte NO_ERROR                      = 0;
    public static final byte INTERNAL_ERROR                = 1;
    public static final byte MARKER_ERROR                  = 2;
    public static final byte MULTIPLE_ECHO_ERROR           = 3;
    public static final byte LOCKS_ERROR                   = 4;
    public static final byte COMMENT_WITH_SEQ_FIELDS_ERROR = 5;
    public static final byte OTHER_ERROR                   = 6; // means 'status' holds valid error status (not fail)

    private final TokenReader        reader;
    private final ZcodeActionFactory actionFactory;

    // Marker status
    private byte    nextMarker       = 0;    // 5 bit really
    private boolean haveNextMarker   = false;
    private byte    prevMarker       = 0;    // 5 bit really
    private byte    seqEndMarker     = 0;    // 4 bits really
    private boolean haveSeqEndMarker = false;

    // parser state-machine
//    private boolean atSeqStart     = true;  // PRESEQUENCE
//    private boolean isAddressing   = false; // ADDRESSING_ASYNC
//    private boolean firstCommand   = true;  // PRESEQUENCE
//    private boolean commandStarted = false; // COMMAND_ASYNC

//    private boolean skipToNL = false; // SKIP_TO_NL
////  private boolean needSendError  = false;
//    private boolean needEndSeq     = false; // POSTSEQUENCE
//    private boolean needCloseParen = false; // --- ???
//
//    // command execution state-machine
//    private boolean commandComplete = false; // COMMAND_EXEC ...
//    private boolean needsAction     = false; // COMMAND_ASYNC ...

    // AND/OR logic
    private boolean isFailed          = false;              // COMMAND_FAILED
    private boolean isSkippingHandler = false;              // COMMAND_SKIP
    private int     parenCounter      = 0;
    private byte    error             = NO_ERROR;           // 3 bit really
    private byte    status            = ZcodeStatus.SUCCESS;

    // Sequence-start info
    private ZcodeLockSet locks    = ZcodeLockSet.allLocked();
    private boolean      hasLocks = false;
    private int          echo     = 0;                       // 16 bit
    private boolean      hasEcho  = false;

    // Execution state
    private boolean activated = false;
    private boolean locked    = false;

    private ZcodeAction currentAction;

    enum State {
        PRESEQUENCE,
        ADDRESSING_INCOMPLETE,
        ADDRESSING_NEEDS_ACTION,
        ADDRESSING_COMPLETE,
        PRECOMMAND,
        COMMAND_INCOMPLETE,
        COMMAND_NEEDS_ACTION,
        COMMAND_COMPLETE,
        COMMAND_FAILED,
        COMMAND_SKIP,
        SEQUENCE_SKIP,
        POSTSEQUENCE,
        ERROR
    }

    private State state;

    public SemanticParser(final TokenReader reader, ZcodeActionFactory actionFactory) {
        this.reader = reader;
        this.actionFactory = actionFactory;
        this.currentAction = null;
        this.state = State.PRESEQUENCE;
    }

    public ZcodeAction getAction() {
        // currentAction is always nulled after execution
        if (currentAction == null || !currentAction.needsPerforming()) {
            currentAction = findNextAction();
        }
        return currentAction;
    }

    private ZcodeAction findNextAction() {
        switch (state) {
        case PRESEQUENCE:
            dealWithTokenBufferFlags();
            if (!haveNextMarker) {
                return actionFactory.needsTokens(this);
            }

            state = parseSequenceLevel();

            if (state == State.PRESEQUENCE) {
                if (reader.getFirstReadToken().getKey() == Zchars.Z_ADDRESSING) {
                    return setStateAndAction(State.ADDRESSING_INCOMPLETE, () -> actionFactory.addressing(this));
                }
                return setStateAndAction(State.PRECOMMAND, () -> actionFactory.firstCommand(this));
            }
            // force iteration to handle ERROR / SEQUENCE_SKIP
            return actionFactory.noAction(this);

        case ADDRESSING_INCOMPLETE:
            return actionFactory.addressingMoveAlong(this);
        case ADDRESSING_NEEDS_ACTION:
            state = State.ADDRESSING_INCOMPLETE;
            return actionFactory.addressingMoveAlong(this);
        case PRECOMMAND:
            return actionFactory.noAction(this);
        case COMMAND_INCOMPLETE:
            return actionFactory.noAction(this);
        case COMMAND_FAILED:
            return actionFactory.noAction(this);
        case COMMAND_SKIP:
            return actionFactory.noAction(this);
        case SEQUENCE_SKIP:
            return actionFactory.noAction(this);
        case POSTSEQUENCE:
            return actionFactory.noAction(this);
        case ERROR:
            return actionFactory.noAction(this);
        }
    }

    private ZcodeAction setStateAndAction(State state, Supplier<ZcodeAction> s) {
        this.state = State.COMMAND_INCOMPLETE;
        return s.get();
    }

    @Override
    public void actionPerformed(ActionType type) {
        currentAction = null;
        switch (type) {
        case ADDRESSING:
        case ADDRESSING_MOVEALONG:
            state = (error == NO_ERROR) ? State.POSTSEQUENCE : State.ERROR;
            break;
        case CLOSE_PAREN:
            break;
        case FIRST_COMMAND:
            break;
        case COMMAND:
            break;
        case COMMAND_MOVEALONG:
            break;
        case END_SEQUENCE:
            break;
        case ERROR:
            break;
        case NEEDS_TOKENS:
            break;
        case NO_ACTION:
            break;
        }
    }

//        if (needSendError) {
//            return actionFactory.error(this, error);
//        }
//
//        // check if we've just completed a command/address - if so, tidy up.
//        if (commandComplete && !skipToNL)
//
//        {
//            commandComplete = false;
//            commandStarted = false;
//
//            skipToMarker();
//
//            if (isAddressing) {
//                reader.flushFirstReadToken();
//                resetToSequence();
//            } else {
//                // read the separator or NL, flush it, and process it
//                final byte marker = reader.getFirstReadToken().getKey();
//                reader.flushFirstReadToken();
//                flowControl(marker);
//            }
//            prevMarker = nextMarker;
//            findNextMarker();
//        }
//
//        if (needEndSeq) {
//            return actionFactory.endSequence(this, p -> {
////                unlock(zcode);
//                seqEndSent();
//            });
//        }
//        if (needCloseParen) {
//            return actionFactory.closeParen(this, p -> closeParenSent());
//        }
//
//        dealWithTokenBufferFlags();
//
//        if (!haveNextMarker) {
//            return actionFactory.needsTokens(this);
//        }
//
//        // deals with '_' and '%',
//        if (atSeqStart) {
//            atSeqStart = false;
//            parseSequenceLevel();
//            if (skipToNL && error != NO_ERROR) {
//                skipToSeqEnd();
////                needSendError = true;
//                return actionFactory.error(this, error);
//            }
//        }
//
//        if (skipToNL) {
//            skipToSeqEnd();
////            needSendError = false;
//            error = NO_ERROR;
//            return actionFactory.noAction(this);
//        }
//
//        if (isAddressing) {
//            if (commandStarted) {
//                if (needsAction) {
//                    return actionFactory.addressingMoveAlong(this, p -> clearNeedsAction());
//                }
//                return actionFactory.noAction(this);
//            }
//            if (error != NO_ERROR) {
////                needSendError = true;
//                return actionFactory.error(this, error);
//            }
//            return actionFactory.addressing(this);
//        }
//
//        if (commandStarted) {
//            if (needsAction) {
//                return actionFactory.commandMoveAlong(this, p -> clearNeedsAction());
//            }
//            return actionFactory.noAction(this);
//        }
//
//        if (error != NO_ERROR) {
//            skipToNL = true;
//            skipToSeqEnd();
////            needSendError = true;
//            return actionFactory.error(this, error);
//        }
//
//        if (firstCommand) {
//            return actionFactory.firstCommand(this, p -> startCommand());
//        }
//
//        if (isSkippingHandler || isFailed) {
//            commandComplete = true;
//            return actionFactory.noAction(this);
//        }
//
//        return actionFactory.runCommand(this, prevMarker, p -> startCommand(), null);
//    }

    private void flowControl(final byte marker) {
        if (ZcodeTokenBuffer.isSequenceEndMarker(marker)) {
            // only expects \n here - other error tokens are handled elsewhere
            resetToSequence();
            needEndSeq = true;
        } else {
            if (marker == ZcodeTokenizer.CMD_END_ORELSE) {
                if (isFailed) {
                    if (parenCounter == 0) {
                        isFailed = false;
                    }
                } else if (!isSkippingHandler) {
                    isSkippingHandler = true;
                    parenCounter = 0;
                }
            } else if (marker == ZcodeTokenizer.CMD_END_OPEN_PAREN) {
                if (isFailed || isSkippingHandler) {
                    parenCounter++;
                }
            } else if (marker == ZcodeTokenizer.CMD_END_CLOSE_PAREN) {
                if (isFailed) {
                    if (parenCounter != 0) {
                        parenCounter--;
                    } else {
                        needCloseParen = true;
                    }
                } else if (isSkippingHandler) {
                    if (parenCounter == 0) {
                        isSkippingHandler = false;
                    } else {
                        parenCounter--;
                    }
                }
            }
        }
    }

    /**
     * Checks the buffer's flags and makes sure we've identified the next marker and the next sequence marker, if available.
     *
     * Makes sure the buffer flag's readerBlocked status is up-to-date.
     */
    private void dealWithTokenBufferFlags() {
        final ZcodeTokenBufferFlags flags = reader.getFlags();
        if (flags.getAndClearMarkerWritten()) {
            if (!haveNextMarker) {
                findNextMarker();
            }
        }

        if (flags.getAndClearSeqMarkerWritten()) {
            if (!haveSeqEndMarker) {
                findSeqEndMarker();
            }
        }

        if (haveNextMarker) {
            flags.clearReaderBlocked();
        } else {
            flags.setReaderBlocked();
        }
    }

    private void findNextMarker() {
        final TokenBufferIterator it = reader.iterator();

        Optional<ReadToken> token;
        for (token = it.next(); token.isPresent() && !token.get().isMarker(); token = it.next()) {
        }

        if (token.isPresent()) {
            nextMarker = token.get().getKey();
            haveNextMarker = true;

            if (ZcodeTokenBuffer.isSequenceEndMarker(nextMarker)) {
                assignSeqEndMarker(nextMarker);
            }
        } else {
            haveNextMarker = false;
        }
    }

    private void findSeqEndMarker() {
        final TokenBufferIterator it    = reader.iterator();
        Optional<ReadToken>       token = it.next();

        for (token = it.next(); token.isPresent() && !ZcodeTokenBuffer.isSequenceEndMarker(token.get().getKey()); token = it.next()) {
        }

        if (token.isPresent()) {
            assignSeqEndMarker(token.get().getKey());
        } else {
            haveSeqEndMarker = false;
        }
    }

    /**
     * Sets <i>seqEndMarker</i>, <i>haveSeqEndMarker</i>, and (if not normal end) <i>error</i> appropriately.
     *
     * @param newSeqEndMarker the new end-marker to assign
     */
    private void assignSeqEndMarker(byte newSeqEndMarker) {
        seqEndMarker = newSeqEndMarker;
        haveSeqEndMarker = true;
        if (newSeqEndMarker != ZcodeTokenizer.NORMAL_SEQUENCE_END) {
            error = MARKER_ERROR;
        }
    }

    /**
     * Updates reader index to point at the next marker token.
     *
     * Note: only call if marker is known to exist! Otherwise it just eats all the known tokens.
     */
    private void skipToMarker() {
        final TokenBufferIterator it = reader.iterator();
        for (Optional<ReadToken> token = it.next(); token.isPresent() && !token.get().isMarker(); token = it.next()) {
            it.flushBuffer();
        }
        findNextMarker();
    }

    /**
     * Fast-forwards the reader to the token after the next sequence end marker.
     */
    private void skipToSeqEnd() {
        if (!haveSeqEndMarker) {
            return;
        }

        TokenBufferIterator it = reader.iterator();
        for (Optional<ReadToken> token = it.next(); !ZcodeTokenBuffer.isSequenceEndMarker(token.get().getKey()); token = it.next()) {
            it.flushBuffer();
        }

        reader.flushFirstReadToken();

        resetToSequence();
        findNextMarker();
        findSeqEndMarker();
    }

    /**
     * Captures the initial fields - ECHO and LOCKS - at the beginning of a sequence, with lots of error checking. Also detects and error-checks COMMENT sequences.
     *
     * Presumes 'nextMarker' is up-to-date.
     *
     * @return the new state: ERROR, SEQUENCE_SKIP (for comments), or PRESEQUENCE (implying no error, normal sequence)
     */
    private State parseSequenceLevel() {
        ReadToken first = reader.getFirstReadToken();
        while (first.getKey() == Zchars.Z_ECHO || first.getKey() == Zchars.Z_LOCKS) {
            if (first.getKey() == Zchars.Z_ECHO) {
                if (hasEcho) {
                    error = MULTIPLE_ECHO_ERROR;
                    return State.ERROR;
                }
                echo = first.getData16();
                hasEcho = true;
            } else if (first.getKey() == Zchars.Z_LOCKS) {
                if (hasLocks) {
                    error = LOCKS_ERROR;
                    return State.ERROR;
                }
                if (first.getDataSize() > ZcodeLocks.LOCK_BYTENUM) {
                    error = LOCKS_ERROR;
                    return State.ERROR;
                }
                locks = ZcodeLockSet.from(first.blockIterator());
                hasLocks = true;
            }
            reader.flushFirstReadToken();
            first = reader.getFirstReadToken();
        }

        switch (first.getKey()) {
        case Zchars.Z_COMMENT:
            if (!ZcodeTokenBuffer.isSequenceEndMarker(nextMarker)) {
                error = INTERNAL_ERROR;
                return State.ERROR;
            }
            if (nextMarker != ZcodeTokenizer.NORMAL_SEQUENCE_END) {
                error = MARKER_ERROR;
                return State.ERROR;
            }
            if (hasEcho || hasLocks) {
                error = COMMENT_WITH_SEQ_FIELDS_ERROR;
                return State.ERROR;
            }
            return State.SEQUENCE_SKIP;
        }
        return State.PRESEQUENCE;
    }

    private void resetToSequence() {
        locks = ZcodeLockSet.allLocked();
        state = State.PRESEQUENCE;
        prevMarker = 0;
        hasLocks = false;
        hasEcho = false;
        isFailed = false;
        isSkippingHandler = false;
        parenCounter = 0;
        findSeqEndMarker();
    }

//    @Override
//    public void startCommand() {
//        this.commandStarted = true;
//        this.firstCommand = false;
//        this.status = ZcodeStatus.SUCCESS;
//    }

    @Override
    public byte getSeqEndMarker() {
        return seqEndMarker;
    }

    @Override
    public void errorSent() {
//        needSendError = false;
        error = NO_ERROR;
    }

//    @Override
//    public void seqEndSent() {
//        needEndSeq = false;
//    }
//
//    @Override
//    public void closeParenSent() {
//        needCloseParen = false;
//    }
//
//    @Override
//    public void clearNeedsAction() {
//        needsAction = false;
//    }

    ////////////////////////////////
    // Sequence Start state: locks and echo. Defined in {@link ParseState}.

    @Override
    public boolean canLock(final Zcode zcode) {
        return locked || zcode.canLock(locks);
    }

    @Override
    public boolean lock(Zcode zcode) {
        if (locked) {
            return true;
        }
        locked = zcode.lock(locks);
        return locked;
    }

    @Override
    public void unlock(Zcode zcode) {
        zcode.unlock(locks);
        this.locked = false;
    }

    @Override
    public boolean hasEcho() {
        return hasEcho;
    }

    @Override
    public int getEcho() {
        return echo;
    }

    ////////////////////////////////
    // Defined in {@link ContextView}

    @Override
    public TokenReader getReader() {
        return reader;
    }

    @Override
    public void setCommandComplete(final boolean b) {
        switch (state) {
        case ADDRESSING_COMPLETE:
        case ADDRESSING_INCOMPLETE:
            state = b ? State.ADDRESSING_COMPLETE : State.ADDRESSING_INCOMPLETE;
            break;
        case COMMAND_COMPLETE:
        case COMMAND_INCOMPLETE:
            state = b ? State.COMMAND_COMPLETE : State.COMMAND_INCOMPLETE;
            break;
        case COMMAND_FAILED:
//            error = OTHER_ERROR; // ?? should we do this? What error?
//            state = State.ERROR;
//            break;
        default:
            throw new IllegalStateException("Invalid state transition");
        }
    }

    @Override
    public boolean isCommandComplete() {
        return state == State.COMMAND_COMPLETE || state == State.ADDRESSING_COMPLETE;
    }

    @Override
    public boolean isActivated() {
        return activated;
    }

    @Override
    public void activate() {
        activated = true;
    }

    @Override
    public void setStatus(byte status) {
        this.status = status;
        if (ZcodeStatus.isError(status)) {
            error = OTHER_ERROR;
            state = State.ERROR;
        } else if (ZcodeStatus.isFailure(status)) {
            switch (state) {
            case COMMAND_COMPLETE:
            case COMMAND_INCOMPLETE:
                state = State.COMMAND_FAILED;
                parenCounter = 0;
                break;
            default:
                throw new IllegalStateException("Invalid state transition");
            }
        }
    }

    @Override
    public void needsAction() {
        switch (state) {
        case ADDRESSING_INCOMPLETE:
            state = State.ADDRESSING_NEEDS_ACTION;
            break;
        case COMMAND_INCOMPLETE:
            state = State.COMMAND_NEEDS_ACTION;
            break;
        default:
            throw new IllegalStateException("Invalid state transition");
        }
    }
}
