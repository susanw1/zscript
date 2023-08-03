/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ZSCRIPT_SEMANTICPARSER_SEMANTICPARSER_HPP_
#define SRC_MAIN_C___ZSCRIPT_SEMANTICPARSER_SEMANTICPARSER_HPP_

#include "../ZscriptIncludes.hpp"
#include "../tokenizer/TokenRingBuffer.hpp"
#include "../tokenizer/ZscriptTokenizer.hpp"
#include "../Zscript.hpp"
#include "../execution/LockSet.hpp"

#include "../execution/ZscriptAction.hpp"

namespace Zscript {
namespace GenericCore {

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
};

template<class ZP>
class SemanticParser {
    uint8_t nextMarker = 0;    // 5 bit really
    bool haveNextMarker = false;
    uint8_t seqEndMarker = 0;    // 4 bits really
    bool haveSeqEndMarker = false;

    TokenRingBuffer<ZP> *const buffer;
    uint8_t channelIndex = 0;
    uint8_t parenCounter = 0;    // 8 bit
    bool hasSentStatusB = false;

    // Sequence-start info - note, bools are only usefully "true" during PRESEQUENCE - merge into state?
    LockSet<ZP> locks = LockSet<ZP>::allLocked();
    bool hasLocks = false;
    uint16_t echo = 0;                       // 16 bit
    bool hasEchoB = false;

    // Execution state
    bool activated = false;
    bool locked = false;

    SemanticActionType currentAction; // 4 bits

    State state; // 5 bit
    /**
     * Checks the buffer's flags and makes sure we've identified the next marker and the next sequence marker, if available.
     *
     * Makes sure the buffer flag's readerBlocked status is up-to-date.
     */
    void dealWithTokenBufferFlags() {
        ZcodeTokenBufferFlags<ZP> *flags = buffer->getFlags();

        if (state != State::COMMAND_INCOMPLETE && state != State::COMMAND_NEEDS_ACTION && flags->getAndClearSeqMarkerWritten()) {
            if (!haveSeqEndMarker) {
                seekMarker(true, false);
            }
        }
        if (flags->getAndClearMarkerWritten()) {
            if (!haveNextMarker) {
                seekMarker(false, false);
            }
        }

        if (haveNextMarker) {
            flags->clearReaderBlocked();
        } else {
            flags->setReaderBlocked();
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
    OptionalRingBufferToken<ZP> seekMarker(bool seekSeqEnd, bool flush) {
        RingBufferTokenIterator<ZP> it = buffer->R_iterator();
        OptionalRingBufferToken<ZP> token;

        bool first = true;
        for (token = it.next(buffer); token.isPresent && !(token.token.isSequenceEndMarker(buffer) || (!seekSeqEnd && token.token.isMarker(buffer))); token = it.next(buffer)) {
            if (flush) {
                it.flushBuffer(buffer);
            } else if (first && seekSeqEnd && token.token.isMarker(buffer)) {
                first = false;
                nextMarker = token.token.getKey(buffer);
                haveNextMarker = true;
            }
        }

        if (seekSeqEnd) {
            if (token.isPresent) {
                assignSeqEndMarker(token.token.getKey(buffer));    // Literally only this case can happen mid command to cause an error...
            } else {
                haveSeqEndMarker = false;
            }
            if (flush || first) {
                haveNextMarker = haveSeqEndMarker;
                nextMarker = seqEndMarker;
            }
        } else {
            haveNextMarker = token.isPresent;
            if (token.isPresent) {
                nextMarker = token.token.getKey(buffer);
                if (TokenRingBuffer<ZP>::isSequenceEndMarker(nextMarker)) {
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
    void assignSeqEndMarker(uint8_t newSeqEndMarker) {
        seqEndMarker = newSeqEndMarker;
        haveSeqEndMarker = true;
        if (newSeqEndMarker != ZscriptTokenizer<ZP>::NORMAL_SEQUENCE_END) {
            state = State::ERROR_TOKENIZER;
        }
    }

    /**
     * Used when reader is pointing at a marker: flush the marker and immediately seek the next marker to update cached marker state.
     */
    void flushMarkerAndSeekNext() {
        bool isSeq = buffer->R_getFirstReadToken().isSequenceEndMarker(buffer);
        buffer->R_flushFirstReadToken(); // unsafe
        seekMarker(isSeq, false); // safe - cached marker state updated
    }

    /**
     * Checks whether the reader has a marker, and returns WAIT_FOR_TOKENS if not
     */
    SemanticActionType checkNeedsTokens() {
        if (!haveNextMarker || !haveSeqEndMarker) {
            dealWithTokenBufferFlags();
            if (!haveNextMarker) {
                return SemanticActionType::WAIT_FOR_TOKENS;
            }
        }
        return SemanticActionType::INVALID;
    }

    /**
     * Attempts to skip the sequence, flushing the marker if it is reached. Returns true if the reader is now on a new sequence.
     */
    bool skipSequence() {
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
    void discardSequenceToEnd() {
        seekMarker(true, true);
    }

    /**
     * Returns the next marker, and discards tokens up to, but not including it
     */
    uint8_t skipToAndGetNextMarker() {
        seekMarker(false, true);
        return nextMarker;
    }

    bool isInErrorState() {
        return state == ERROR_TOKENIZER || state == ERROR_MULTIPLE_ECHO || state == ERROR_LOCKS || state == ERROR_COMMENT_WITH_SEQ_FIELDS
                || state == ERROR_STATUS;
    }
public:
    void actionPerformed(SemanticActionType type) {
        currentAction = SemanticActionType::INVALID;

        switch (type) {
        case SemanticActionType::END_SEQUENCE:
            resetToSequence();
            //            seekMarker(true, false); // Might not be necessary?
            break;
        case SemanticActionType::ERROR:
            if (skipSequence()) {
                resetToSequence();
            } else {
                state = SEQUENCE_SKIP;
            }
            break;
        default:
            break;
        }
    }

    SemanticParser(TokenRingBuffer<ZP> *buffer) :
            buffer(buffer), currentAction(SemanticActionType::INVALID), state(PRESEQUENCE) {
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

    ZscriptAction<ZP> getAction() {
        // currentAction is always nulled after action execution
        while (currentAction == SemanticActionType::INVALID || currentAction == SemanticActionType::GO_AROUND) {
            if (checkNeedsTokens() == SemanticActionType::WAIT_FOR_TOKENS) {
                return ZscriptAction<ZP>(this, SemanticActionType::WAIT_FOR_TOKENS);
            }
            // We now haveNextMarker (or we returned to wait for tokens).
            currentAction = findNextAction();
        }
        return ZscriptAction<ZP>(this, currentAction);
    }

private:

    SemanticActionType findNextAction() {
        SemanticActionType action;
        switch (state) {
        case PRESEQUENCE:
            // expecting buffer to be pointing at start of sequence.
            parseSequenceLevel();
            if (state == PRESEQUENCE) {
                // expecting buffer to be pointing at first token after lock/echo preamble.
                if (buffer->R_getFirstReadToken().getKey(buffer) == Zchars::Z_ADDRESSING) {
                    state = ADDRESSING_INCOMPLETE;
                    return SemanticActionType::INVOKE_ADDRESSING;
                }
                // Until first command is run (and starts presuming it will complete), assert that the command state is INcomplete
                state = COMMAND_INCOMPLETE;
                return SemanticActionType::RUN_FIRST_COMMAND;
            }
            // force iteration to handle ERROR / SEQUENCE_SKIP
            return SemanticActionType::GO_AROUND;

        case ADDRESSING_INCOMPLETE:
            return SemanticActionType::WAIT_FOR_ASYNC;

        case ADDRESSING_NEEDS_ACTION:
            state = ADDRESSING_INCOMPLETE;
            return SemanticActionType::ADDRESSING_MOVEALONG;

        case ADDRESSING_COMPLETE:
            state = SEQUENCE_SKIP;
            skipSequence();
            return SemanticActionType::END_SEQUENCE;

        case COMMAND_INCOMPLETE:
            return SemanticActionType::WAIT_FOR_ASYNC;

        case COMMAND_NEEDS_ACTION:
            state = COMMAND_INCOMPLETE;
            return SemanticActionType::COMMAND_MOVEALONG;

        case COMMAND_COMPLETE_NEEDS_TOKENS:
            case COMMAND_COMPLETE:
            case COMMAND_FAILED:
            case COMMAND_SKIP:
            // record the marker we know is at the end of the last command (and, def not an error), then move past it
            action = flowControl(skipToAndGetNextMarker());
            if (state == COMMAND_INCOMPLETE && !seekSecondMarker()) {
                state = COMMAND_COMPLETE_NEEDS_TOKENS;
                return SemanticActionType::WAIT_FOR_TOKENS;
            } else {
                if (action != SemanticActionType::RUN_COMMAND) {
                    flushMarkerAndSeekNext();
                }
                return action;
            }

        case SEQUENCE_SKIP:
            if (!skipSequence()) {
                return SemanticActionType::WAIT_FOR_TOKENS;
            }
            resetToSequence();
            return SemanticActionType::GO_AROUND;

        case ERROR_COMMENT_WITH_SEQ_FIELDS:
            case ERROR_LOCKS:
            case ERROR_MULTIPLE_ECHO:
            case ERROR_TOKENIZER:
            discardSequenceToEnd();
            return SemanticActionType::ERROR;
        case ERROR_STATUS:
            if (skipSequence()) {
                return SemanticActionType::END_SEQUENCE;
            } else {
                return SemanticActionType::WAIT_FOR_TOKENS;
            }
        default:
            return SemanticActionType::ERROR; //unreachable hopefully
        }
    }

private:
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
    SemanticActionType flowControl(uint8_t marker) {
        if (isInErrorState()) {
            return SemanticActionType::GO_AROUND;
        }

        if (TokenRingBuffer<ZP>::isSequenceEndMarker(marker)) {
            // only expects \n here - other error tokens are handled elsewhere in ERROR state
            return SemanticActionType::END_SEQUENCE;
        }

        if (state == COMMAND_COMPLETE_NEEDS_TOKENS) {
            state = COMMAND_INCOMPLETE;
            return SemanticActionType::RUN_COMMAND;
        }

        if (marker == ZscriptTokenizer<ZP>::CMD_END_ORELSE) {
            if (state == COMMAND_FAILED) {
                if (parenCounter == 0) {
                    // command failure was caught, and we hit an OR, so start running next command
                    state = COMMAND_COMPLETE;
                }
            } else if (state == COMMAND_COMPLETE) {
                // previous command succeeded, but we hit an OR so skip next one.
                state = COMMAND_SKIP;
                parenCounter = 0;
            } else {
                // no action for COMMAND_SKIP, just keep skipping
            }
        } else if (marker == ZscriptTokenizer<ZP>::CMD_END_OPEN_PAREN) {
            // increment paren counter (actually not relevant when prev command succeeeded!)
            parenCounter++;
        } else if (marker == ZscriptTokenizer<ZP>::CMD_END_CLOSE_PAREN) {
            if (state == COMMAND_FAILED) {
                // keep track of matched parens that we've skipped; only send ')' to match '(' for commands that were executed
                if (parenCounter == 0) {
                    return SemanticActionType::CLOSE_PAREN;
                }
                parenCounter--;
            } else if (state == COMMAND_SKIP) {
                // keep track of matched parens that we've skipped; only begin executing cmds when we exit the group where the skipping began.
                if (parenCounter == 0) {
                    state = COMMAND_COMPLETE;
                } else {
                    parenCounter--;
                }
            }
        }

        // handles AND, '(' and some ')' cases
        if (state == COMMAND_COMPLETE) {
            hasSentStatusB = false;
            state = COMMAND_INCOMPLETE;
            return SemanticActionType::RUN_COMMAND;
        }

        // this happens when we're skipping cmds
        return SemanticActionType::GO_AROUND;
    }

    /**
     * Tool for dealing with finding we've got a separator marker (say, ANDTHEN) but we haven't got the tokens yet to processs the next command. We could store that marker, and
     * move on, but that requires memory and state. Instead, this method tells us if we've got the second token, otherwise we go into COMMAND_COMPLETE_NEEDS_TOKENS.
     *
     * @return true if second marker exists; false otherwise
     */
    bool seekSecondMarker() {
        // starts pointing at a marker - we want to know if there's another one after it, or if we're out of tokens!
        RingBufferTokenIterator<ZP> it = buffer->R_iterator();

        // skip the current (Marker) token, and then
        it.next(buffer);

        for (OptionalRingBufferToken<ZP> token = it.next(buffer); token.isPresent; token = it.next(buffer)) {
            if (token.token.isMarker(buffer)) {
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
    void parseSequenceLevel() {
        RingBufferToken<ZP> first = buffer->R_getFirstReadToken();
        while (first.getKey(buffer) == Zchars::Z_ECHO || first.getKey(buffer) == Zchars::Z_LOCKS) {
            if (first.getKey(buffer) == Zchars::Z_ECHO) {
                if (hasEchoB) {
                    state = ERROR_MULTIPLE_ECHO;
                    return;
                }
                echo = first.getData16(buffer);
                hasEchoB = true;
            } else if (first.getKey(buffer) == Zchars::Z_LOCKS) {
                if (hasLocks || first.getDataSize(buffer) > ZP::lockByteCount) {
                    state = ERROR_LOCKS;
                    return;
                }
                locks = LockSet<ZP>::from(first.blockIterator(buffer), buffer);
                hasLocks = true;
            }
            buffer->R_flushFirstReadToken(); // safe as key is checked, not a Marker
            first = buffer->R_getFirstReadToken();
        }

        if (first.getKey(buffer) == Zchars::Z_COMMENT) {
            if (nextMarker != ZscriptTokenizer<ZP>::NORMAL_SEQUENCE_END) {
                state = ERROR_TOKENIZER;
                return;
            }
            if (hasEchoB || hasLocks) {
                state = ERROR_COMMENT_WITH_SEQ_FIELDS;
                return;
            }
            state = SEQUENCE_SKIP;
        }
    }

    void resetToSequence() {
        locks = LockSet<ZP>::allLocked();
        state = PRESEQUENCE;
        hasSentStatusB = false;
        hasLocks = false;
        hasEchoB = false;
        parenCounter = 0;
    }

public:
    void setChannelIndex(uint8_t channelIndex) {
        this->channelIndex = channelIndex;
    }
    TokenRingBuffer<ZP>* getBuffer() {
        return buffer;
    }

    ////////////////////////////////
    // Sequence Start state: locks and echo. Defined in {@link ParseState}.

    /**
     * Operates on the first token on the  If it is a marker, returns and discards it, otherwise returns 0 and does nothing.
     */
    uint8_t takeCurrentMarker() {
        uint8_t marker = buffer->R_getFirstReadToken().getKey(buffer);
        if (!TokenRingBuffer<ZP>::isMarker(marker)) {
            return 0;
        }
        flushMarkerAndSeekNext();
        return marker;
    }

    uint8_t getErrorStatus() {
        uint8_t tokenizerError;
        uint8_t statusValue;
        switch (state) {
        case ERROR_COMMENT_WITH_SEQ_FIELDS:
            return INVALID_COMMENT;
        case ERROR_LOCKS:
            return INVALID_LOCKS;
        case ERROR_MULTIPLE_ECHO:
            return INVALID_ECHO;
        case ERROR_TOKENIZER:
            tokenizerError = buffer->R_getFirstReadToken().getKey(buffer);

            if (tokenizerError == ZscriptTokenizer<ZP>::ERROR_BUFFER_OVERRUN) {
                statusValue = BUFFER_OVR_ERROR;
            } else if (tokenizerError == ZscriptTokenizer<ZP>::ERROR_CODE_FIELD_TOO_LONG) {
                statusValue = FIELD_TOO_LONG;
            } else if (tokenizerError == ZscriptTokenizer<ZP>::ERROR_CODE_ILLEGAL_TOKEN) {
                statusValue = ILLEGAL_KEY;
            } else if (tokenizerError == ZscriptTokenizer<ZP>::ERROR_CODE_ODD_BIGFIELD_LENGTH) {
                statusValue = ODD_LENGTH;
            } else if (tokenizerError == ZscriptTokenizer<ZP>::ERROR_CODE_STRING_ESCAPING) {
                statusValue = ESCAPING_ERROR;
            } else if (tokenizerError == ZscriptTokenizer<ZP>::ERROR_CODE_STRING_NOT_TERMINATED) {
                statusValue = UNTERMINATED_STRING;
            } else {
                statusValue = (INTERNAL_ERROR);
            }
            return statusValue;
        default:
            return INTERNAL_ERROR;    // should be unreachable
        }
    }

    bool canLock(Zscript<ZP> *z) {
        return locked || z->canLock(&locks);
    }

    bool lock(Zscript<ZP> *z) {
        if (locked) {
            return true;
        }
        locked = z->lock(&locks);
        return locked;
    }

    bool hasSentStatus() {
        return hasSentStatusB;
    }

    void unlock(Zscript<ZP> *z) {
        z->unlock(&locks);
        this->locked = false;
    }

    bool hasEcho() {
        return hasEchoB;
    }

    uint16_t getEcho() {
        return echo;
    }

    ////////////////////////////////
    // Defined in {@link ContextView}

    void setCommandComplete(bool b) {
        switch (state) {
        case ADDRESSING_COMPLETE:
            case ADDRESSING_INCOMPLETE:
            state = b ? ADDRESSING_COMPLETE : ADDRESSING_INCOMPLETE;
            break;
        case COMMAND_COMPLETE:
            case COMMAND_INCOMPLETE:
            state = b ? COMMAND_COMPLETE : COMMAND_INCOMPLETE;
            break;
        default:
            //Unreachable
            break;
        }
    }

    bool isCommandComplete() {
        return state == COMMAND_COMPLETE || state == ADDRESSING_COMPLETE;
    }

    bool isActivated() {
        return activated;
    }

    void activate() {
        activated = true;
    }

    void setStatus(uint8_t status) {
        hasSentStatusB = true;
        if (ResponseStatusUtils<ZP>::isError(status)) {
            state = ERROR_STATUS;
        } else if (ResponseStatusUtils<ZP>::isFailure(status)) {
            switch (state) {
            case COMMAND_COMPLETE:
                case COMMAND_INCOMPLETE:
                state = COMMAND_FAILED;
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
                //Unreachable
                break;
            }
        }
    }

    void notifyNeedsAction() {
        switch (state) {
        case ADDRESSING_INCOMPLETE:
            case ADDRESSING_NEEDS_ACTION:
            state = ADDRESSING_NEEDS_ACTION;
            break;
        case COMMAND_INCOMPLETE:
            case COMMAND_NEEDS_ACTION:
            state = COMMAND_NEEDS_ACTION;
            break;
        default:
            //Unreachable
            break;
        }
    }

    void silentSucceed() {
        hasSentStatusB = true;
    }

    uint8_t getChannelIndex() {
        return channelIndex;
    }

    bool isEmpty() {
        return buffer->R_getFirstReadToken().isMarker(buffer);
    }
    AsyncActionNotifier<ZP> getAsyncActionNotifier() {
        return AsyncActionNotifier<ZP>(this);
    }
};

}
}

#endif /* SRC_MAIN_C___ZSCRIPT_SEMANTICPARSER_SEMANTICPARSER_HPP_ */
