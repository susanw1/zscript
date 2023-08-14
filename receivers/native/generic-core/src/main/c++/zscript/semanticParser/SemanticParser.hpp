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
#include "../execution/LockSet.hpp"

#include "../execution/ZscriptAction.hpp"

namespace Zscript {

template<class ZP>
class Zscript;
namespace GenericCore {

enum class SemanticParserState : uint8_t {
    PRESEQUENCE,

#ifdef ZSCRIPT_SUPPORT_ADDRESSING
    ADDRESSING_INCOMPLETE,
    ADDRESSING_NEEDS_ACTION,
    ADDRESSING_COMPLETE,
#endif
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
    ERROR_STATUS,
    STOPPING,
    STOPPED
};

template<class ZP>
class SemanticParser {
    TokenRingReader<ZP> reader;
    uint16_t echo = 0;
    LockSet<ZP> locks = LockSet<ZP>::allLocked();
    uint8_t channelIndex = 0;
    //Basic attempt has been made to improve ordering on bitpacking...
    SemanticActionType currentAction :4; // 4 bits
    uint8_t parenCounter :4;    // 4 bit

    uint8_t nextMarker :5;    // 5 bit really
    bool haveNextMarker :1;
    bool haveSeqEndMarker :1;
    bool hasSentStatusB :1;

    SemanticParserState state :5; // 5 bit
    bool hasEchoB :1;
    // Execution state
    bool activated :1;

    bool locked :1;

    uint8_t seqEndMarker :4;    // 4 bits really

public:
    bool freeBool :1;

    SemanticParser(TokenRingBuffer<ZP> *buffer) :
            reader(buffer->getReader()),
                    currentAction(SemanticActionType::INVALID), parenCounter(0),
                    nextMarker(0), haveNextMarker(false), haveSeqEndMarker(false), hasSentStatusB(false),
                    state(SemanticParserState::PRESEQUENCE), hasEchoB(false), activated(false), locked(false),
                    seqEndMarker(0), freeBool(false) {
    }
private:

    uint8_t fetchNextMarker() {
        return nextMarker | 0xE0;
    }
    uint8_t fetchSeqEndMarker() {
        return seqEndMarker | 0xF0;
    }
    uint8_t seqMarkerToNextMarker() {
        return seqEndMarker | 0x10;
    }

    /**
     * Checks the buffer's flags and makes sure we've identified the next marker and the next sequence marker, if available.
     *
     * Makes sure the buffer flag's readerBlocked status is up-to-date.
     */
    void dealWithTokenBufferFlags() {
        TokenBufferFlags<ZP> *flags = reader.getFlags();

        if (state != SemanticParserState::COMMAND_INCOMPLETE && state != SemanticParserState::COMMAND_NEEDS_ACTION && flags->getAndClearSeqMarkerWritten()) {
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
        RingBufferTokenIterator<ZP> it = reader.rawIterator();
        OptionalRingBufferToken<ZP> token;

        bool first = true;
        for (token = it.next(reader.asBuffer());
                token.isPresent && !(token.token.isSequenceEndMarker(reader.asBuffer()) || (!seekSeqEnd && token.token.isMarker(reader.asBuffer())));
                token = it.next(reader.asBuffer())) {
            if (flush) {
                it.flushBuffer(reader.asBuffer());
            } else if (first && seekSeqEnd && token.token.isMarker(reader.asBuffer())) {
                first = false;
                nextMarker = (token.token.getKey(reader.asBuffer())) & 0x1F;
                haveNextMarker = true;
            }
        }

        if (seekSeqEnd) {
            if (token.isPresent) {
                assignSeqEndMarker(token.token.getKey(reader.asBuffer()));    // Literally only this case can happen mid command to cause an error...
            } else {
                haveSeqEndMarker = false;
            }
            if (flush || first) {
                haveNextMarker = haveSeqEndMarker;
                nextMarker = seqMarkerToNextMarker() & 0x1F;
            }
        } else {
            haveNextMarker = token.isPresent;
            if (token.isPresent) {
                nextMarker = token.token.getKey(reader.asBuffer()) & 0x1F;
                if (TokenRingBuffer<ZP>::isSequenceEndMarker(fetchNextMarker())) {
                    assignSeqEndMarker(fetchNextMarker());
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
        seqEndMarker = newSeqEndMarker & 0xF;
        haveSeqEndMarker = true;
        if (newSeqEndMarker != ZscriptTokenizer<ZP>::NORMAL_SEQUENCE_END) {
            state = SemanticParserState::ERROR_TOKENIZER;
        }
    }

    /**
     * Used when reader is pointing at a marker: flush the marker and immediately seek the next marker to update cached marker state.
     */
    void flushMarkerAndSeekNext() {
        bool isSeq = reader.getFirstReadToken().isSequenceEndMarker(reader.asBuffer());
        reader.flushFirstReadToken(); // unsafe
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
        return fetchNextMarker();
    }
public:
    void emptyActionPerformed() {
        currentAction = SemanticActionType::INVALID;
    }
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
                state = SemanticParserState::SEQUENCE_SKIP;
            }
            break;
        default:
            break;
        }
    }

    // VisibleForTesting
    SemanticParserState getState() {
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
        case SemanticParserState::PRESEQUENCE:
            // expecting buffer to be pointing at start of sequence.
            parseSequenceLevel();
            if (state == SemanticParserState::PRESEQUENCE) {
                // expecting buffer to be pointing at first token after lock/echo preamble.

#ifdef ZSCRIPT_SUPPORT_ADDRESSING
                if (reader.getFirstReadToken().getKey(reader.asBuffer()) == Zchars::Z_ADDRESSING) {
                    state = SemanticParserState::ADDRESSING_INCOMPLETE;
                    return SemanticActionType::INVOKE_ADDRESSING;
                }
#endif
                // Until first command is run (and starts presuming it will complete), assert that the command state is INcomplete
                state = SemanticParserState::COMMAND_INCOMPLETE;
                return SemanticActionType::RUN_FIRST_COMMAND;
            }
            // force iteration to handle ERROR / SemanticParserState::SEQUENCE
            return SemanticActionType::GO_AROUND;

#ifdef ZSCRIPT_SUPPORT_ADDRESSING
        case SemanticParserState::ADDRESSING_INCOMPLETE:
            return SemanticActionType::WAIT_FOR_ASYNC;

        case SemanticParserState::ADDRESSING_NEEDS_ACTION:
            state = SemanticParserState::ADDRESSING_INCOMPLETE;
            return SemanticActionType::ADDRESSING_MOVEALONG;

        case SemanticParserState::ADDRESSING_COMPLETE:
            state = SemanticParserState::PRESEQUENCE;
            skipSequence();
            return SemanticActionType::END_SEQUENCE;
#endif

        case SemanticParserState::COMMAND_INCOMPLETE:
            return SemanticActionType::WAIT_FOR_ASYNC;

        case SemanticParserState::COMMAND_NEEDS_ACTION:
            state = SemanticParserState::COMMAND_INCOMPLETE;
            return SemanticActionType::COMMAND_MOVEALONG;

        case SemanticParserState::COMMAND_COMPLETE_NEEDS_TOKENS:
            case SemanticParserState::COMMAND_COMPLETE:
            case SemanticParserState::COMMAND_FAILED:
            case SemanticParserState::COMMAND_SKIP:
            // record the marker we know is at the end of the last command (and, def not an error), then move past it
            action = flowControl(skipToAndGetNextMarker());
            if (state == SemanticParserState::COMMAND_INCOMPLETE && !seekSecondMarker()) {
                state = SemanticParserState::COMMAND_COMPLETE_NEEDS_TOKENS;
                return SemanticActionType::WAIT_FOR_TOKENS;
            } else {
                if (action != SemanticActionType::RUN_COMMAND) {
                    flushMarkerAndSeekNext();
                }
                return action;
            }

        case SemanticParserState::SEQUENCE_SKIP:
            if (!skipSequence()) {
                return SemanticActionType::WAIT_FOR_TOKENS;
            }
            resetToSequence();
            return SemanticActionType::GO_AROUND;

        case SemanticParserState::ERROR_COMMENT_WITH_SEQ_FIELDS:
            case SemanticParserState::ERROR_LOCKS:
            case SemanticParserState::ERROR_MULTIPLE_ECHO:
            case SemanticParserState::ERROR_TOKENIZER:
            discardSequenceToEnd();
            return SemanticActionType::ERROR;
        case SemanticParserState::ERROR_STATUS:
            if (skipSequence()) {
                return SemanticActionType::END_SEQUENCE;
            } else {
                return SemanticActionType::WAIT_FOR_TOKENS;
            }
        case SemanticParserState::STOPPING:
            if (!skipSequence()) {
                return SemanticActionType::WAIT_FOR_TOKENS;
            }
            state = SemanticParserState::STOPPED;
            return SemanticActionType::END_SEQUENCE;

        case SemanticParserState::STOPPED:
            return SemanticActionType::STOPPED;
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

        if (state == SemanticParserState::COMMAND_COMPLETE_NEEDS_TOKENS) {
            state = SemanticParserState::COMMAND_INCOMPLETE;
            return SemanticActionType::RUN_COMMAND;
        }

        if (marker == ZscriptTokenizer<ZP>::CMD_END_ORELSE) {
            if (state == SemanticParserState::COMMAND_FAILED) {
                if (parenCounter == 0) {
                    // command failure was caught, and we hit an OR, so start running next command
                    state = SemanticParserState::COMMAND_COMPLETE;
                }
            } else if (state == SemanticParserState::COMMAND_COMPLETE) {
                // previous command succeeded, but we hit an OR so skip next one.
                state = SemanticParserState::COMMAND_SKIP;
                parenCounter = 0;
            } else {
                // no action for COMMAND_SKIP, just keep skipping
            }
        } else if (marker == ZscriptTokenizer<ZP>::CMD_END_OPEN_PAREN) {
            // increment paren counter (actually not relevant when prev command succeeeded!)
            parenCounter++;
        } else if (marker == ZscriptTokenizer<ZP>::CMD_END_CLOSE_PAREN) {
            if (state == SemanticParserState::COMMAND_FAILED) {
                // keep track of matched parens that we've skipped; only send ')' to match '(' for commands that were executed
                if (parenCounter == 0) {
                    return SemanticActionType::CLOSE_PAREN;
                }
                parenCounter--;
            } else if (state == SemanticParserState::COMMAND_SKIP) {
                // keep track of matched parens that we've skipped; only begin executing cmds when we exit the group where the skipping began.
                if (parenCounter == 0) {
                    state = SemanticParserState::COMMAND_COMPLETE;
                } else {
                    parenCounter--;
                }
            }
        }

        // handles AND, '(' and some ')' cases
        if (state == SemanticParserState::COMMAND_COMPLETE) {
            hasSentStatusB = false;
            state = SemanticParserState::COMMAND_INCOMPLETE;
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
        RingBufferTokenIterator<ZP> it = reader.rawIterator();

        // skip the current (Marker) token, and then
        it.next(reader.asBuffer());

        for (OptionalRingBufferToken<ZP> token = it.next(reader.asBuffer()); token.isPresent; token = it.next(reader.asBuffer())) {
            if (token.token.isMarker(reader.asBuffer())) {
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
     * Leaves the parser in: ERROR_*, SemanticParserState::SEQUENCE (for comments), or PRESEQUENCE (implying no error, normal sequence)
     */
    void parseSequenceLevel() {
        bool hasLocks = false;
        RingBufferToken<ZP> first = reader.getFirstReadToken();
        while (first.getKey(reader.asBuffer()) == Zchars::Z_ECHO || first.getKey(reader.asBuffer()) == Zchars::Z_LOCKS) {
            if (first.getKey(reader.asBuffer()) == Zchars::Z_ECHO) {
                if (hasEchoB) {
                    state = SemanticParserState::ERROR_MULTIPLE_ECHO;
                    return;
                }
                echo = first.getData16(reader.asBuffer());
                hasEchoB = true;
            } else if (first.getKey(reader.asBuffer()) == Zchars::Z_LOCKS) {
                if (hasLocks || first.hasSizeGreaterThan(reader.asBuffer(), ZP::lockByteCount)) {
                    state = SemanticParserState::ERROR_LOCKS;
                    return;
                }
                locks = LockSet<ZP>::from(first.blockIterator(reader.asBuffer()));
                hasLocks = true;
            }
            reader.flushFirstReadToken(); // safe as key is checked, not a Marker
            first = reader.getFirstReadToken();
        }

#ifdef ZSCRIPT_DONT_FAST_DISCARD_COMMENTS
        if (first.getKey(reader.asBuffer()) == Zchars::Z_COMMENT) {
            if (fetchNextMarker() != ZscriptTokenizer<ZP>::NORMAL_SEQUENCE_END) {
                state = SemanticParserState::ERROR_TOKENIZER;
                return;
            }
            if (hasEchoB || hasLocks) {
                state = SemanticParserState::ERROR_COMMENT_WITH_SEQ_FIELDS;
                return;
            }
            state = SemanticParserState::SEQUENCE_SKIP;
        }
#endif
    }

    void resetToSequence() {
        locks = LockSet<ZP>::allLocked();
        if (state != SemanticParserState::STOPPED && state != SemanticParserState::STOPPING) {
            state = SemanticParserState::PRESEQUENCE;
        }
        hasSentStatusB = false;
        hasEchoB = false;
        parenCounter = 0;
    }

public:
    void setChannelIndex(uint8_t channelIndex) {
        this->channelIndex = channelIndex;
    }
    TokenRingReader<ZP> getReader() {
        return reader;
    }

    ////////////////////////////////
    // Sequence Start state: locks and echo. Defined in {@link ParseState}.

    /**
     * Operates on the first token on the  If it is a marker, returns and discards it, otherwise returns 0 and does nothing.
     */
    uint8_t takeCurrentMarker() {
        uint8_t marker = reader.getFirstReadToken().getKey(reader.asBuffer());
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
        case SemanticParserState::ERROR_COMMENT_WITH_SEQ_FIELDS:
            return INVALID_COMMENT;
        case SemanticParserState::ERROR_LOCKS:
            return INVALID_LOCKS;
        case SemanticParserState::ERROR_MULTIPLE_ECHO:
            return INVALID_ECHO;
        case SemanticParserState::ERROR_TOKENIZER:
            tokenizerError = reader.getFirstReadToken().getKey(reader.asBuffer());

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

    bool canLock() {
        return locked || Zscript<ZP>::zscript.canLock(&locks);
    }

    bool lock() {
        if (locked) {
            return true;
        }
        locked = Zscript<ZP>::zscript.lock(&locks);
        return locked;
    }

    bool hasSentStatus() {
        return hasSentStatusB;
    }

    void unlock() {
        Zscript<ZP>::zscript.unlock(&locks);
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
        case SemanticParserState::STOPPING:
            break;
#ifdef ZSCRIPT_SUPPORT_ADDRESSING
        case SemanticParserState::ADDRESSING_COMPLETE:
            case SemanticParserState::ADDRESSING_INCOMPLETE:
            state = b ? SemanticParserState::ADDRESSING_COMPLETE : SemanticParserState::ADDRESSING_INCOMPLETE;
            break;
#endif
        case SemanticParserState::COMMAND_COMPLETE:
            case SemanticParserState::COMMAND_INCOMPLETE:
            state = b ? SemanticParserState::COMMAND_COMPLETE : SemanticParserState::COMMAND_INCOMPLETE;
            break;
        default:
            //Unreachable
            break;
        }
    }

    bool isCommandComplete() {
        return state == SemanticParserState::COMMAND_COMPLETE

#ifdef ZSCRIPT_SUPPORT_ADDRESSING
                || state == SemanticParserState::ADDRESSING_COMPLETE
#endif
        ;
    }

    bool isActivated() {
        return activated;
    }

    void activate() {
        activated = true;
    }

    void deActivate() {
        activated = false;
    }

    void setStatus(uint8_t status) {
        hasSentStatusB = true;
        if (ResponseStatusUtils<ZP>::isError(status)) {
            state = SemanticParserState::ERROR_STATUS;
        } else if (ResponseStatusUtils<ZP>::isFailure(status)) {
            switch (state) {
            case SemanticParserState::COMMAND_COMPLETE:
                case SemanticParserState::COMMAND_INCOMPLETE:
                state = SemanticParserState::COMMAND_FAILED;
                parenCounter = 0;
                break;
            case SemanticParserState::ERROR_COMMENT_WITH_SEQ_FIELDS:
                case SemanticParserState::ERROR_LOCKS:
                case SemanticParserState::ERROR_MULTIPLE_ECHO:
                case SemanticParserState::ERROR_STATUS:
                case SemanticParserState::ERROR_TOKENIZER:
                // ignore: command cannot report failure after an ERROR.
                break;
            default:
                //Unreachable
                break;
            }
        }
    }
    bool requestStatusChange(uint8_t status) {
        if (hasSentStatusB) {
            return false;
        }
        hasSentStatusB = true;
        if (state == SemanticParserState::STOPPED || state == SemanticParserState::STOPPING) {
            return true;
        }
        if (ResponseStatusUtils<ZP>::isSuccess(status)) {
            return true;
        }

        if (ResponseStatusUtils<ZP>::isError(status)) {
            state = SemanticParserState::ERROR_STATUS;
            return true;
        }

        if (ResponseStatusUtils<ZP>::isFailure(status)) {
            switch (state) {
            case SemanticParserState::COMMAND_COMPLETE:
                case SemanticParserState::COMMAND_INCOMPLETE:
                state = SemanticParserState::COMMAND_FAILED;
                parenCounter = 0;
                return true;
            case SemanticParserState::ERROR_COMMENT_WITH_SEQ_FIELDS:
                case SemanticParserState::ERROR_LOCKS:
                case SemanticParserState::ERROR_MULTIPLE_ECHO:
                case SemanticParserState::ERROR_STATUS:
                case SemanticParserState::ERROR_TOKENIZER:
                // ignore: command cannot report failure after an ERROR.
                return false;
            default:
                //unreachable...
                break;
            }
        }
        return false;
    }

    void notifyNeedsAction() {
        switch (state) {
#ifdef ZSCRIPT_SUPPORT_ADDRESSING
        case SemanticParserState::ADDRESSING_INCOMPLETE:
            case SemanticParserState::ADDRESSING_NEEDS_ACTION:
            state = SemanticParserState::ADDRESSING_NEEDS_ACTION;
            break;
#endif
        case SemanticParserState::COMMAND_INCOMPLETE:
            case SemanticParserState::COMMAND_NEEDS_ACTION:
            state = SemanticParserState::COMMAND_NEEDS_ACTION;
            break;
        default:
            //Unreachable
            break;
        }
    }

    void stop() {
        if (state == SemanticParserState::PRESEQUENCE || state == SemanticParserState::STOPPED) {
            state = SemanticParserState::STOPPED;
        } else {
            state = SemanticParserState::STOPPING;
        }
    }

    void resume() {
        if (state == SemanticParserState::STOPPED) {
            seekMarker(true, false);
            state = SemanticParserState::PRESEQUENCE;
        } else {
            //unreachable...
        }
    }

    bool isRunning() {
        return state != SemanticParserState::STOPPED && state != SemanticParserState::STOPPING;
    }

    void silentSucceed() {
        hasSentStatusB = true;
    }

    uint8_t getChannelIndex() {
        return channelIndex;
    }

    bool isEmpty() {
        return reader.getFirstReadToken().isMarker(reader.asBuffer());
    }
    AsyncActionNotifier<ZP> getAsyncActionNotifier() {
        return AsyncActionNotifier<ZP>(this);
    }

    bool isInErrorState() {
        return state == SemanticParserState::ERROR_TOKENIZER
                || state == SemanticParserState::ERROR_MULTIPLE_ECHO
                || state == SemanticParserState::ERROR_LOCKS
                || state == SemanticParserState::ERROR_COMMENT_WITH_SEQ_FIELDS
                || state == SemanticParserState::ERROR_STATUS;
    }

    bool isFailed() {
        return state == SemanticParserState::COMMAND_FAILED;
    }
};

}
}

#endif /* SRC_MAIN_C___ZSCRIPT_SEMANTICPARSER_SEMANTICPARSER_HPP_ */
