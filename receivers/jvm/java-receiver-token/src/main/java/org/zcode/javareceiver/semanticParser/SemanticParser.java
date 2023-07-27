package org.zcode.javareceiver.semanticParser;

import java.util.Optional;
import java.util.function.Predicate;

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
    // 6 bytes + 1 pointer

    private final TokenReader        reader;
    private final ZcodeActionFactory actionFactory;

    private class MarkerCache {
        private byte    nextMarker       = 0;    // 5 bit really
        private boolean haveNextMarker   = false;
        private byte    seqEndMarker     = 0;    // 4 bits really
        private boolean haveSeqEndMarker = false;

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
         * Scans the reader for the next Marker (or sequence-end Marker). If {@code flush} is set, then all tokens prior to the one returned are flushed.
         *
         * Also, in all circumstances, it ensures that the Marker cache values {@link #haveSeqEndMarker}/{@link #seqEndMarker} and {@link #haveNextMarker}/{@link #nextMarker} are
         * all correct and up-to-date.
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
         * Used when reader is pointing at a marker: flush the marker and immediately seek the next marker to update cached marker state.
         */
        public void flushMarkerAndSeekNext() {
            boolean isSeq = reader.getFirstReadToken().isSequenceEndMarker();
            reader.flushFirstReadToken(); // unsafe
            seekMarker(isSeq, false); // safe - cached marker state updated
        }

        /**
         * Checks whether the reader has a marker, and returns WAIT_FOR_TOKENS if not
         */
        public ActionType checkNeedsTokens() {
            if (!haveNextMarker || !haveSeqEndMarker) {
                dealWithTokenBufferFlags();
                if (!haveNextMarker) {
                    return ActionType.WAIT_FOR_TOKENS;
                }
            }
            return ActionType.INVALID;
        }

        /**
         * Attempts to skip the sequence, flushing the marker if it is reached. Returns true if the reader is now on a new sequence.
         */
        public boolean skipSequence() {
            seekMarker(true, true);
            if (haveSeqEndMarker) {
                flushMarkerAndSeekNext();
                return true;
            }
            return false;
        }

        /**
         * Drops as many tokens inside the sequence as possible, without discarding the sequence end marker.
         */
        public void discardSequenceToEnd() {
            seekMarker(true, true);
        }

        /**
         * Returns the next marker, and discards tokens up to, but not including it
         */
        public byte skipToAndGetNextMarker() {
            markerCache.seekMarker(false, true);
            if (!haveNextMarker) {
                throw new IllegalStateException();
            }
            return nextMarker;
        }

        /**
         * Operates on the first token on the reader. If it is a marker, returns and discards it, otherwise returns 0 and does nothing.
         */
        public byte takeCurrentMarker() {
            byte marker = reader.getFirstKey();
            if (!ZcodeTokenBuffer.isMarker(marker)) {
                return 0;
            }
            flushMarkerAndSeekNext();
            return marker;
        }
    }

    // Marker status
    private final MarkerCache markerCache = new MarkerCache();

    private int     parenCounter  = 0;    // 8 bit
    private boolean hasSentStatus = false;

    // Sequence-start info - note, booleans are only usefully "true" during PRESEQUENCE - merge into state?
    private ZcodeLockSet locks    = ZcodeLockSet.allLocked();
    private boolean      hasLocks = false;
    private int          echo     = 0;                       // 16 bit
    private boolean      hasEcho  = false;

    // Execution state
    private boolean activated = false;
    private boolean locked    = false;

    private ActionType currentAction; // 4 bits

    private State state; // 5 bit

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

    public SemanticParser(final TokenReader reader, ZcodeActionFactory actionFactory) {
        this.reader = reader;
        this.actionFactory = actionFactory;
        this.currentAction = ActionType.INVALID;
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
        while (currentAction == ActionType.INVALID || currentAction == ActionType.GO_AROUND) {
            if (markerCache.checkNeedsTokens() == ActionType.WAIT_FOR_TOKENS) {
                return actionFactory.waitForTokens(this);
            }
            // We now haveNextMarker (or we returned to wait for tokens).
            currentAction = findNextAction();
        }
        return actionFactory.ofType(this, currentAction);
    }

    private ActionType findNextAction() {
        switch (state) {
        case PRESEQUENCE:
            // expecting buffer to be pointing at start of sequence.
            parseSequenceLevel();
            if (state == State.PRESEQUENCE) {
                // expecting buffer to be pointing at first token after lock/echo preamble.
                if (reader.getFirstReadToken().getKey() == Zchars.Z_ADDRESSING) {
                    return setStateAndAction(State.ADDRESSING_INCOMPLETE, ActionType.INVOKE_ADDRESSING);
                }
                // Until first command is run (and starts presuming it will complete), assert that the command state is INcomplete
                return setStateAndAction(State.COMMAND_INCOMPLETE, ActionType.RUN_FIRST_COMMAND);
            }
            // force iteration to handle ERROR / SEQUENCE_SKIP
            return ActionType.GO_AROUND;

        case ADDRESSING_INCOMPLETE:
            return ActionType.WAIT_FOR_ASYNC;

        case ADDRESSING_NEEDS_ACTION:
            state = State.ADDRESSING_INCOMPLETE;
            return ActionType.ADDRESSING_MOVEALONG;

        case ADDRESSING_COMPLETE:
            if (markerCache.skipSequence()) {
                resetToSequence();
            } else {
                state = State.SEQUENCE_SKIP;
            }
            return ActionType.END_SEQUENCE;

        case COMMAND_INCOMPLETE:
            return ActionType.WAIT_FOR_ASYNC;

        case COMMAND_NEEDS_ACTION:
            state = State.COMMAND_INCOMPLETE;
            return ActionType.COMMAND_MOVEALONG;

        case COMMAND_COMPLETE_NEEDS_TOKENS:
        case COMMAND_COMPLETE:
        case COMMAND_FAILED:
        case COMMAND_SKIP:
            // record the marker we know is at the end of the last command (and, def not an error), then move past it
            ActionType action = flowControl(markerCache.skipToAndGetNextMarker());
            if (state == State.COMMAND_INCOMPLETE && !seekSecondMarker()) {
                return setStateAndAction(State.COMMAND_COMPLETE_NEEDS_TOKENS, ActionType.WAIT_FOR_TOKENS);
            } else {
                if (action != ActionType.RUN_COMMAND) {
                    markerCache.flushMarkerAndSeekNext();
                }
                return action;
            }

        case SEQUENCE_SKIP:
            if (!markerCache.skipSequence()) {
                return ActionType.WAIT_FOR_TOKENS;
            }
            resetToSequence();
            return ActionType.GO_AROUND;

        case ERROR_COMMENT_WITH_SEQ_FIELDS:
        case ERROR_LOCKS:
        case ERROR_MULTIPLE_ECHO:
        case ERROR_TOKENIZER:
            markerCache.discardSequenceToEnd();
            return ActionType.ERROR;
        case ERROR_STATUS:
            if (markerCache.skipSequence()) {
                return ActionType.END_SEQUENCE;
            } else {
                return ActionType.WAIT_FOR_TOKENS;
            }
        default:
            throw new IllegalStateException();
        }
    }

    private ActionType setStateAndAction(State state, ActionType a) {
        this.state = State.COMMAND_INCOMPLETE;
        return a;
    }

    @Override
    public void actionPerformed(ActionType type) {
        currentAction = ActionType.INVALID;

        switch (type) {
        case INVOKE_ADDRESSING:
        case ADDRESSING_MOVEALONG:
            break;
        case END_SEQUENCE:
            resetToSequence();
//            seekMarker(true, false); // Might not be necessary?
            break;
        case ERROR:
            if (markerCache.skipSequence()) {
                resetToSequence();
            } else {
                state = State.SEQUENCE_SKIP;
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
        case INVALID:
            throw new IllegalStateException();
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
    private ActionType flowControl(final byte marker) {
        if (isInErrorState()) {
            return ActionType.GO_AROUND;
        }

        if (ZcodeTokenBuffer.isSequenceEndMarker(marker)) {
            // only expects \n here - other error tokens are handled elsewhere in ERROR state
            return ActionType.END_SEQUENCE;
        }

        if (state == State.COMMAND_COMPLETE_NEEDS_TOKENS) {
            return ActionType.RUN_COMMAND;
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
                    return ActionType.CLOSE_PAREN;
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
            return ActionType.RUN_COMMAND;
        }

        // this happens when we're skipping cmds
        return ActionType.GO_AROUND;
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
            if (markerCache.nextMarker != ZcodeTokenizer.NORMAL_SEQUENCE_END) {
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
    public byte takeCurrentMarker() {
        return markerCache.takeCurrentMarker();
    }

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
