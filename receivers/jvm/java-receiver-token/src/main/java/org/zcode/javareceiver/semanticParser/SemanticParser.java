package org.zcode.javareceiver.semanticParser;

import java.util.Optional;
import java.util.function.Predicate;
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

    private final TokenReader        reader;
    private final ZcodeActionFactory actionFactory;

    // Marker status
    private byte    nextMarker       = 0;    // 5 bit really
    private boolean haveNextMarker   = false;
    private byte    seqEndMarker     = 0;    // 4 bits really
    private boolean haveSeqEndMarker = false;

    private int     parenCounter  = 0;
    private boolean hasSentStatus = false;

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
        COMMAND_INCOMPLETE,
        COMMAND_NEEDS_ACTION,
        COMMAND_COMPLETE_NEEDS_TOKENS,
        COMMAND_COMPLETE,
        COMMAND_FAILED,
        COMMAND_SKIP,
        SEQUENCE_SKIP,
        ERROR_TOKENIZER,
        ERROR_MULTIPLE_ECHO,
        ERROR_LOCKS,
        ERROR_COMMENT_WITH_SEQ_FIELDS,
        ERROR_STATUS
    }

    private boolean isInErrorState() {
        return state == State.ERROR_TOKENIZER || state == State.ERROR_MULTIPLE_ECHO || state == State.ERROR_LOCKS || state == State.ERROR_COMMENT_WITH_SEQ_FIELDS
                || state == State.ERROR_STATUS;
    }

    private State state;

    public SemanticParser(final TokenReader reader, ZcodeActionFactory actionFactory) {
        this.reader = reader;
        this.actionFactory = actionFactory;
        this.currentAction = null;
        this.state = State.PRESEQUENCE;
    }

    // VisibleForTesting
    State getState() {
        return state;
    }

    /**
     * Determines the next action to be performed.
     *
     * @return
     */
    public ZcodeAction getAction() {
        // currentAction is always nulled after action execution
        while (currentAction == null || currentAction.getType() == ActionType.GO_AROUND) {
            if (!haveNextMarker || !haveSeqEndMarker) {
                dealWithTokenBufferFlags();
                if (!haveNextMarker) {
                    // future optimization: if we're in a SEQUENCE_SKIP state, we could burn off tokens regardless to empty the buffer?
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
            parseSequenceLevel();
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
            // TODO should be skip to sequence end....?
            seekMarker(false, true);
            flushMarkerAndSeekNext();
            return actionFactory.endSequence(this);

        case COMMAND_INCOMPLETE:
            return actionFactory.waitForAsync(this);

        case COMMAND_NEEDS_ACTION:
            state = State.COMMAND_INCOMPLETE;
            return actionFactory.commandMoveAlong(this);

        case COMMAND_COMPLETE_NEEDS_TOKENS:
        case COMMAND_COMPLETE:
        case COMMAND_FAILED:
        case COMMAND_SKIP:
            // record the marker we know is at the end of the last command (and, def not an error), then move past it
            ReadToken marker = seekMarker(false, true).get();
            ZcodeAction action = flowControl(marker.getKey());
            if (state == State.COMMAND_INCOMPLETE && !seekSecondMarker()) {
                return setStateAndAction(State.COMMAND_COMPLETE_NEEDS_TOKENS, () -> actionFactory.waitForTokens(this));
            } else {
                flushMarkerAndSeekNext();
                return action;
            }

        case SEQUENCE_SKIP:
            seekMarker(true, true);
            if (!haveSeqEndMarker) {
                return actionFactory.waitForTokens(this);
            }

            // flush the end marker - buffer is now pointing at the next sequence, if any
            flushMarkerAndSeekNext();
            resetToSequence();
            return actionFactory.goAround(this);

        case ERROR_COMMENT_WITH_SEQ_FIELDS:
        case ERROR_LOCKS:
        case ERROR_MULTIPLE_ECHO:
        case ERROR_TOKENIZER:
            seekMarker(true, true);
            return actionFactory.error(this);
        case ERROR_STATUS:
            seekMarker(true, true);
            if (haveSeqEndMarker) {
                flushMarkerAndSeekNext();
                return actionFactory.endSequence(this); // end sequence isn't quite right here, as the actionPerformed code is completely wrong
            } else {
                return actionFactory.waitForTokens(this);
            }
        default:
            throw new IllegalStateException();
        }
    }

    /**
     * Used when reader is pointing at a marker: flush the marker and immediately seek the next marker to update cached marker state.
     */
    private void flushMarkerAndSeekNext() {
        boolean isSeq = reader.getFirstReadToken().isSequenceEndMarker();
        reader.flushFirstReadToken(); // unsafe
        seekMarker(isSeq, false); // safe - cached marker state updated
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
//            state = (error == NO_ERROR) ? State.POSTSEQUENCE : State.ERROR;
            break;
        case END_SEQUENCE:
            resetToSequence();
//            seekMarker(true, false); // Might not be necessary?
            break;
        case ERROR:
            seekMarker(true, true);
            if (!haveSeqEndMarker) {
                state = State.SEQUENCE_SKIP;
            } else {
                flushMarkerAndSeekNext();
                resetToSequence();
            }
            break;
        case CLOSE_PAREN:
        case RUN_FIRST_COMMAND:
        case RUN_COMMAND:
        case COMMAND_MOVEALONG:
            break;
        case WAIT_FOR_TOKENS:
        case WAIT_FOR_ASYNC:
        case GO_AROUND:
            break;
        }
    }

    /**
     * Handles cases COMMAND_COMPLETE, COMMAND_FAILED, COMMAND_SKIP with the following logical operator, and decides how to proceed. This method does not interact with the token
     * buffer, but generates the new State/Action based on the current state and the Marker just before this command.
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
     * @return an appropriate action: END_SEQUENCE, RUN_COMMAND, CLOSE_PAREN, GO_AROUND
     */
    private ZcodeAction flowControl(final byte marker) {
        if (isInErrorState()) {
            return actionFactory.goAround(this);
        }

        if (ZcodeTokenBuffer.isSequenceEndMarker(marker)) {
            // only expects \n here - other error tokens are handled elsewhere in ERROR state
            return actionFactory.endSequence(this);
        }

        if (state == State.COMMAND_COMPLETE_NEEDS_TOKENS) {
            return actionFactory.runCommand(this, marker);
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
            hasSentStatus = false;
            state = State.COMMAND_INCOMPLETE;
            return actionFactory.runCommand(this, marker);
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

        if (state != State.COMMAND_INCOMPLETE && state != State.COMMAND_NEEDS_ACTION && flags.getAndClearSeqMarkerWritten()) {
            if (!haveSeqEndMarker) {
                seekMarker(true, false);
            }
        }
        if (flags.getAndClearMarkerWritten()) {
            if (!haveNextMarker) {
                seekMarker(false, false);
            }
        }

        if (haveNextMarker) {
            flags.clearReaderBlocked();
        } else {
            flags.setReaderBlocked();
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
            state = State.ERROR_TOKENIZER;
        }
    }

    /**
     * Scans the reader for the next Marker (or sequence-end Marker). If {@code flush} is set, then all tokens prior to the one returned are flushed.
     *
     * Also, in all circumstances, it ensures that the Marker cache values {@link #haveSeqEndMarker}/{@link #seqEndMarker} and {@link #haveNextMarker}/{@link #nextMarker} are all
     * correct and up-to-date.
     *
     * If buffer's reader is pointing at a matching Marker already, it just returns that Marker (flush has no effect).
     *
     * @param seekSeqEnd if true, seeks the sequence-end marker, otherwise just seeks any marker
     * @param flush      enables flushing of tokens (up to, but not including the Marker), otherwise buffer is unchanged
     *
     * @return the Marker token that was found, or empty if none
     */
    private Optional<ReadToken> seekMarker(final boolean seekSeqEnd, final boolean flush) {
        final TokenBufferIterator it = reader.iterator();
        Optional<ReadToken>       token;

        Predicate<ReadToken> p = seekSeqEnd ? ReadToken::isSequenceEndMarker : ReadToken::isMarker;

        boolean first = true;
        for (token = it.next(); token.filter(p.negate()).isPresent(); token = it.next()) {
            if (flush) {
                it.flushBuffer();
            } else if (first && seekSeqEnd && token.get().isMarker()) {
                first = false;
                nextMarker = token.get().getKey();
                haveNextMarker = true;
            }
        }

        if (seekSeqEnd) {
            if (token.isPresent()) {
                assignSeqEndMarker(token.get().getKey());// Literally only this case can happen mid command to cause an error...
            } else {
                haveSeqEndMarker = false;
            }
            if (flush || first) {
                haveNextMarker = haveSeqEndMarker;
                nextMarker = seqEndMarker;
            }
        } else {
            haveNextMarker = token.isPresent();
            if (token.isPresent()) {
                nextMarker = token.get().getKey();
                if (ZcodeTokenBuffer.isSequenceEndMarker(nextMarker)) {
                    assignSeqEndMarker(nextMarker);
                }
            }
        }

        return token;
    }

    /**
     * Tool for dealing with finding we've got a separator marker (say, ANDTHEN) but we haven't got the tokens yet to processs the next command. We could store that marker, and
     * move on, but that requires memory and state. Instead, this method tells us if we've got the second token, otherwise we go into COMMAND_COMPLETE_NEEDS_TOKENS.
     *
     * @return true if second marker exists; false otherwise
     */
    private boolean seekSecondMarker() {
        // starts pointing at a marker - we want to know if there's another one after it, or if we're out of tokens!
        final TokenBufferIterator it = reader.iterator();

        // skip the current (Marker) token, and then
        it.next();

        for (Optional<ReadToken> token = it.next(); token.isPresent(); token = it.next()) {
            if (token.get().isMarker()) {
                return true;
            }
        }
        return false;
    }

    /**
     * Captures the initial fields - ECHO and LOCKS - at the beginning of a sequence, with lots of error checking. Also detects and error-checks COMMENT sequences.
     *
     * Buffer ends up pointing at first token after ECHO/LOCK.
     *
     * Presumes 'nextMarker' is up-to-date.
     *
     * Leaves the parser in: ERROR_*, SEQUENCE_SKIP (for comments), or PRESEQUENCE (implying no error, normal sequence)
     */
    private void parseSequenceLevel() {
        ReadToken first = reader.getFirstReadToken();
        while (first.getKey() == Zchars.Z_ECHO || first.getKey() == Zchars.Z_LOCKS) {
            if (first.getKey() == Zchars.Z_ECHO) {
                if (hasEcho) {
                    state = State.ERROR_MULTIPLE_ECHO;
                    return;
                }
                echo = first.getData16();
                hasEcho = true;
            } else if (first.getKey() == Zchars.Z_LOCKS) {
                if (hasLocks || first.getDataSize() > ZcodeLocks.LOCK_BYTENUM) {
                    state = State.ERROR_LOCKS;
                    return;
                }
                locks = ZcodeLockSet.from(first.blockIterator());
                hasLocks = true;
            }
            reader.flushFirstReadToken(); // safe as key is checked, not a Marker
            first = reader.getFirstReadToken();
        }

        if (first.getKey() == Zchars.Z_COMMENT) {
            if (nextMarker != ZcodeTokenizer.NORMAL_SEQUENCE_END) {
                state = State.ERROR_TOKENIZER;
                return;
            }
            if (hasEcho || hasLocks) {
                state = State.ERROR_COMMENT_WITH_SEQ_FIELDS;
                return;
            }
            state = State.SEQUENCE_SKIP;
        }
    }

    private void resetToSequence() {
        locks = ZcodeLockSet.allLocked();
        state = State.PRESEQUENCE;
        hasLocks = false;
        hasEcho = false;
        parenCounter = 0;
    }

    ////////////////////////////////
    // Sequence Start state: locks and echo. Defined in {@link ParseState}.

    @Override
    public byte getErrorStatus() {
        switch (state) {
        case ERROR_COMMENT_WITH_SEQ_FIELDS:
            return ZcodeStatus.INVALID_COMMENT;
        case ERROR_LOCKS:
            return ZcodeStatus.INVALID_LOCKS;
        case ERROR_MULTIPLE_ECHO:
            return ZcodeStatus.INVALID_ECHO;
        case ERROR_TOKENIZER:
            byte tokenizerError = reader.getFirstKey();
            if (!reader.getFirstReadToken().isSequenceEndMarker()) {
                throw new IllegalStateException();
            }
            byte statusValue;

            if (tokenizerError == ZcodeTokenizer.ERROR_BUFFER_OVERRUN) {
                statusValue = ZcodeStatus.BUFFER_OVR_ERROR;
            } else if (tokenizerError == ZcodeTokenizer.ERROR_CODE_FIELD_TOO_LONG) {
                statusValue = ZcodeStatus.FIELD_TOO_LONG;
            } else if (tokenizerError == ZcodeTokenizer.ERROR_CODE_ILLEGAL_TOKEN) {
                statusValue = ZcodeStatus.ILLEGAL_KEY;
            } else if (tokenizerError == ZcodeTokenizer.ERROR_CODE_ODD_BIGFIELD_LENGTH) {
                statusValue = ZcodeStatus.ODD_LENGTH;
            } else if (tokenizerError == ZcodeTokenizer.ERROR_CODE_STRING_ESCAPING) {
                statusValue = ZcodeStatus.ESCAPING_ERROR;
            } else if (tokenizerError == ZcodeTokenizer.ERROR_CODE_STRING_NOT_TERMINATED) {
                statusValue = ZcodeStatus.UNTERMINATED_STRING;
            } else {
                statusValue = (ZcodeStatus.INTERNAL_ERROR);
            }
            return statusValue;
        case ERROR_STATUS:
        default:
            throw new IllegalStateException();
        }
    }

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
    public boolean hasSentStatus() {
        return hasSentStatus;
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
//            state = errorState(OTHER_ERROR);// ?? should we do this? What error?
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
        hasSentStatus = true;
        if (ZcodeStatus.isError(status)) {
            state = State.ERROR_STATUS;
        } else if (ZcodeStatus.isFailure(status)) {
            switch (state) {
            case COMMAND_COMPLETE:
            case COMMAND_INCOMPLETE:
                state = State.COMMAND_FAILED;
                parenCounter = 0;
                break;
            case ERROR_COMMENT_WITH_SEQ_FIELDS:
            case ERROR_LOCKS:
            case ERROR_MULTIPLE_ECHO:
            case ERROR_STATUS:
            case ERROR_TOKENIZER:
                // ignore: command cannot report failure after an ERROR.
                break;
            default:
                throw new IllegalStateException("Invalid state transition");
            }
        }
    }

    @Override
    public void notifyNeedsAction() {
        switch (state) {
        case ADDRESSING_INCOMPLETE:
        case ADDRESSING_NEEDS_ACTION:
            state = State.ADDRESSING_NEEDS_ACTION;
            break;
        case COMMAND_INCOMPLETE:
        case COMMAND_NEEDS_ACTION:
            state = State.COMMAND_NEEDS_ACTION;
            break;
        default:
            throw new IllegalStateException("Invalid state transition");
        }
    }

    @Override
    public void silentSucceed() {
        hasSentStatus = true;
    }
}
