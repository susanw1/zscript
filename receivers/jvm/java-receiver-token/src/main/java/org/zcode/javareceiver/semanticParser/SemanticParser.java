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
//    private boolean isFailed          = false;              // COMMAND_FAILED
//    private boolean isSkippingHandler = false;              // COMMAND_SKIP
    private int  parenCounter = 0;
    private byte error        = NO_ERROR;           // 3 bit really
    private byte status       = ZcodeStatus.SUCCESS;

    // Sequence-start info - note, booleans are only usefully "true" during PRESEQUENCE - remove?
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

    /**
     *
     * @return
     */
    public ZcodeAction getAction() {
        // currentAction is always nulled after action execution
        while (currentAction == null || currentAction.getType() == ActionType.GO_AROUND) {
            if (!haveNextMarker || !haveSeqEndMarker) {
                dealWithTokenBufferFlags();
                if (!haveNextMarker) {
                    return actionFactory.waitForTokens(this);
                }
            }
            // We now haveNextMarker (or we returned to wait for tokens).
            currentAction = findNextAction();
        }
        return currentAction;
    }

    private ZcodeAction findNextAction() {
        switch (state) {
        case PRESEQUENCE:
            // expecting buffer to be pointing at start of sequence.
            state = parseSequenceLevel();
            if (state == State.PRESEQUENCE) {
                // expecting buffer to be pointing at first token after lock/echo preamble.
                if (reader.getFirstReadToken().getKey() == Zchars.Z_ADDRESSING) {
                    return setStateAndAction(State.ADDRESSING_INCOMPLETE, () -> actionFactory.addressing(this));
                }
                // Until first command is run (and starts presuming it will complete), assert that the command state is INcomplete
                return setStateAndAction(State.COMMAND_INCOMPLETE, () -> actionFactory.firstCommand(this));
            }
            // force iteration to handle ERROR / SEQUENCE_SKIP
            return actionFactory.goAround(this);

        case ADDRESSING_INCOMPLETE:
            return actionFactory.waitForAsync(this);

        case ADDRESSING_NEEDS_ACTION:
            state = State.ADDRESSING_INCOMPLETE;
            return actionFactory.addressingMoveAlong(this);

        case ADDRESSING_COMPLETE:
            skipToMarker();
            reader.flushFirstReadToken();
            return actionFactory.endSequence(this);

        case COMMAND_INCOMPLETE:
            return actionFactory.waitForAsync(this);

        case COMMAND_NEEDS_ACTION:
            state = State.COMMAND_INCOMPLETE;
            return actionFactory.commandMoveAlong(this);

//        case PRECOMMAND:
//            state = State.COMMAND_INCOMPLETE;
//            return actionFactory.runCommand(this, prevMarker);

        case COMMAND_COMPLETE:
        case COMMAND_FAILED:
        case COMMAND_SKIP:
            skipToMarker();
            reader.flushFirstReadToken();
            ZcodeAction action = flowControl(nextMarker);
            prevMarker = nextMarker;
            findNextMarker();
            if (action.needsPerforming()) {
                return action; // This implies we were in COMMAND_FAILED, and we still are.
            }
            return actionFactory.goAround(this);

        case SEQUENCE_SKIP:
            if (haveSeqEndMarker) {
                skipToSeqEnd();
                state = State.PRESEQUENCE;
                return actionFactory.goAround(this);
            } else {
                final TokenBufferIterator it = reader.iterator();
                for (Optional<ReadToken> token = it.next(); token.isPresent() && !token.get().isSequenceEndMarker(); token = it.next()) {
                    it.flushBuffer();
                }
                return actionFactory.waitForTokens(this);
            }
        case POSTSEQUENCE:
            // TODO
            return actionFactory.goAround(this);
        case ERROR:
            // TODO
            return actionFactory.goAround(this);
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
        case INVOKE_ADDRESSING:
        case ADDRESSING_MOVEALONG:
            // TODO ??? Looks wrong...
            state = (error == NO_ERROR) ? State.POSTSEQUENCE : State.ERROR;
            break;
        case END_SEQUENCE:
            resetToSequence();
            break;
        case ERROR:
            // TODO!
            break;
        case WAIT_FOR_ASYNC:
        case CLOSE_PAREN:
        case RUN_FIRST_COMMAND:
        case RUN_COMMAND:
        case COMMAND_MOVEALONG:
        case WAIT_FOR_TOKENS:
        case GO_AROUND:
            break;
        }
    }

    /**
     * Handles cases COMMAND_COMPLETE, COMMAND_FAILED, COMMAND_SKIP with the following logical operator, and decides how to proceed.
     *
     * COMMAND_COMPLETE tells us to skip ORELSE cmds; COMMAND_FAILED tells us to skip ANDTHEN cmds; COMMAND_SKIP skips both (ie ends parenthesized groups).
     *
     * Note: parens do two things:
     *
     * 1) in a COMMAND_SKIP state, a ')' exits that state and runs the next command; a '(' neutralizes the matching ')'. This means on encountering an ORELSE inside parens
     * containing succeeding commands, we skip to the ')', whereas if the cmds before the ORELSE failed, we still end up running the cmds after it.
     *
     * 2) in a COMMAND_FAILED state, a '(' neutralizes any ORELSE until the matching ')'. This means a '(...)' acts as a single skipped item when we're skipping due to earlier cmd
     * failure - if we enter in a COMMAND_FAILED state, we need to exit in that state without executing the contents.
     *
     * Net result is that grouped commands act as an atomic unit, being skipped as a group.
     *
     * The parenCounter is interesting: The only way to reach the COMMAND_SKIP state is "successful cmd followed by ORELSE". The only way to reach the COMMAND_FAILED state is
     * "unsuccessful cmd". This means you can't switch from COMMAND_FAILED to COMMAND_SKIP (or vice versa) without running a command in between! And thus the parenCounter is very
     * "local"
     *
     * @param marker the logical operator ending the previous command
     * @return an appropriate action
     */
    private ZcodeAction flowControl(final byte marker) {
        if (ZcodeTokenBuffer.isSequenceEndMarker(marker)) {
            // only expects \n here - other error tokens are handled elsewhere in ERROR state
            return actionFactory.endSequence(this);
        }
        if (marker == ZcodeTokenizer.CMD_END_ORELSE) {
            if (state == State.COMMAND_FAILED) {
                if (parenCounter == 0) {
                    // command failure was caught, and we hit an OR, so start running next command
                    state = State.COMMAND_COMPLETE;
                }
            } else if (state == State.COMMAND_COMPLETE) {
                // previous command succeeded, but we hit an OR so skip next one.
                state = State.COMMAND_SKIP;
                parenCounter = 0;
            } else {
                // no action for COMMAND_SKIP, just keep skipping
            }
        } else if (marker == ZcodeTokenizer.CMD_END_OPEN_PAREN) {
            // increment paren counter (actually not relevant when prev command succeeeded!)
            parenCounter++;
        } else if (marker == ZcodeTokenizer.CMD_END_CLOSE_PAREN) {
            if (state == State.COMMAND_FAILED) {
                // keep track of matched parens that we've skipped; only send ')' to match '(' for commands that were executed
                if (parenCounter == 0) {
                    return actionFactory.closeParen(this);
                }
                parenCounter--;
            } else if (state == State.COMMAND_SKIP) {
                // keep track of matched parens that we've skipped; only begin executing cmds when we exit the group where the skipping began.
                if (parenCounter == 0) {
                    state = State.COMMAND_COMPLETE;
                } else {
                    parenCounter--;
                }
            }
        }

        // handles AND, '(' and some ')' cases
        if (state == State.COMMAND_COMPLETE) {
            state = State.COMMAND_INCOMPLETE;
            return actionFactory.runCommand(this, prevMarker);
        }

        // this happens when we're skipping cmds
        return actionFactory.goAround(this);
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

        for (token = it.next(); token.isPresent() && !token.get().isSequenceEndMarker(); token = it.next()) {
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
        for (Optional<ReadToken> token = it.next(); !token.get().isSequenceEndMarker(); token = it.next()) {
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
     * Buffer ends up pointing at first token after ECHO/LOCK.
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
            case ERROR:
                // ignore: command cannot report failure after an ERROR.
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
