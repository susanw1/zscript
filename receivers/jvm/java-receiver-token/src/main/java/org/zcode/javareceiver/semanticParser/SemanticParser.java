package org.zcode.javareceiver.semanticParser;

import java.util.Optional;

import org.zcode.javareceiver.core.ZcodeLockSet;
import org.zcode.javareceiver.core.ZcodeLocks;
import org.zcode.javareceiver.core.ZcodeStatus;
import org.zcode.javareceiver.tokenizer.TokenBufferIterator;
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
    public static final byte OTHER_ERROR                   = 6;

    private final TokenReader reader;

    private ZcodeLockSet locks    = ZcodeLockSet.allLocked();
    private boolean      hasLocks = false;
    private int          echo     = 0;                       // 16 bit
    private boolean      hasEcho  = false;

    private byte    nextMarker     = 0;    // 5 bit really
    private boolean haveNextMarker = false;

    private byte prevMarker = 0; // 5 bit really

    private byte    seqEndMarker     = 0;    // 4 bits really
    private boolean haveSeqEndMarker = false;

    private boolean atSeqStart = true;

    private boolean isAddressing = false;

    private boolean commandCanComplete = false;
    private boolean commandStarted     = false;
    private boolean needsAction        = false;
    private boolean firstCommand       = true;

    private boolean isFailed          = false;
    private boolean isSkippingHandler = false;
    private int     parenCounter      = 0;

    private byte       error  = NO_ERROR;            // 3 bit really
    private final byte status = ZcodeStatus.SUCCESS; // 3 bit really

    private boolean skipToNL = false;
//    private boolean needSendError  = false;
    private boolean needEndSeq     = false;
    private boolean needCloseParen = false;

    private boolean activated = false;
    private boolean locked    = false;

    private ZcodeAction currentAction;

    public SemanticParser(final TokenReader reader) {
        this.reader = reader;
        this.currentAction = ZcodeAction.noAction(this);
    }

    public ZcodeAction getAction() {
        if (!currentAction.needsPerforming()) {
            currentAction = findNextAction(currentAction);
        }
        return currentAction;
    }

    private ZcodeAction findNextAction(ZcodeAction previousAction) {
//        if (needSendError) {
//            return ZcodeAction.error(this, error);
//        }

        // check if we've just completed a command/address
        if (commandCanComplete && !skipToNL) {
            commandCanComplete = false;
            commandStarted = false;

            skipToMarker();

            if (isAddressing) {
                reader.flushFirstReadToken();
                resetToSequence();
            } else {
                // read the separator or NL, flush it, and process it
                final byte marker = reader.getFirstReadToken().getKey();
                reader.flushFirstReadToken();
                flowControl(marker);
            }
            prevMarker = nextMarker;
            findNextMarker();
        }

        if (needEndSeq) {
            return ZcodeAction.endSequence(this);
        }
        if (needCloseParen) {
            return ZcodeAction.closeParen(this);
        }

        dealWithFlags();

        if (!haveNextMarker) {
            return ZcodeAction.needsTokens(this);
        }

        // deals with '_' and '%',
        if (atSeqStart) {
            atSeqStart = false;
            parseSequenceLevel();
            if (skipToNL && error != NO_ERROR) {
                skipToSeqEnd();
//                needSendError = true;
                return ZcodeAction.error(this, error);
            }
        }

        if (skipToNL) {
            skipToSeqEnd();
//            needSendError = false;
            error = NO_ERROR;
            return ZcodeAction.noAction(this);
        }

        if (isAddressing) {
            if (commandStarted) {
                if (needsAction) {
                    return ZcodeAction.addressingMoveAlong(this);
                }
                return ZcodeAction.noAction(this);
            }
            if (error != NO_ERROR) {
//                needSendError = true;
                return ZcodeAction.error(this, error);
            }
            return ZcodeAction.addressing(this);
        }

        if (commandStarted) {
            if (needsAction) {
                return ZcodeAction.commandMoveAlong(this);
            }
            return ZcodeAction.noAction(this);
        }

        if (error != NO_ERROR) {
            skipToNL = true;
            skipToSeqEnd();
//            needSendError = true;
            return ZcodeAction.error(this, error);
        }

        if (firstCommand) {
            return ZcodeAction.firstCommand(this);
        }

        if (isSkippingHandler || isFailed) {
            commandCanComplete = true;
            return ZcodeAction.noAction(this);
        }

        return ZcodeAction.runCommand(this, prevMarker);
    }

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

    private void dealWithFlags() {
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
        final TokenBufferIterator it    = reader.iterator();
        Optional<ReadToken>       token = it.next();

        while (token.isPresent() && !token.get().isMarker()) {
            token = it.next();
        }

        if (token.isPresent()) {
            nextMarker = token.get().getKey();
            haveNextMarker = true;

            if (ZcodeTokenBuffer.isSequenceEndMarker(nextMarker)) {
                seqEndMarker = nextMarker;
                haveSeqEndMarker = true;
                if (nextMarker != ZcodeTokenizer.NORMAL_SEQUENCE_END) {
                    error = MARKER_ERROR;
                }
            }
        } else {
            haveNextMarker = false;
        }
    }

    private void findSeqEndMarker() {
        final TokenBufferIterator it    = reader.iterator();
        Optional<ReadToken>       token = it.next();

        while (token.isPresent() && !ZcodeTokenBuffer.isSequenceEndMarker(token.get().getKey())) {
            token = it.next();
        }

        if (token.isPresent()) {
            seqEndMarker = token.get().getKey();
            haveSeqEndMarker = true;
            if (token.get().getKey() != ZcodeTokenizer.NORMAL_SEQUENCE_END) {
                error = MARKER_ERROR;
            }
        } else {
            haveSeqEndMarker = false;
        }
    }

    private void skipToMarker() {
        final TokenBufferIterator it = reader.iterator();
        for (Optional<ReadToken> token = it.next(); token.isPresent() && !token.get().isMarker(); token = it.next()) {
            it.flushBuffer();
        }
    }

    private void skipToSeqEnd() {
        if (!haveSeqEndMarker) {
            return;
        }

        byte marker = reader.getFirstReadToken().getKey();
        while (reader.hasReadToken() && !ZcodeTokenBuffer.isSequenceEndMarker(marker)) {
            skipToMarker();
            marker = reader.getFirstReadToken().getKey();
            if (!ZcodeTokenBuffer.isSequenceEndMarker(marker)) {
                reader.flushFirstReadToken();
            }
        }

        if (reader.hasReadToken()) {
            reader.flushFirstReadToken();
        }

        if (ZcodeTokenBuffer.isSequenceEndMarker(marker)) {
            resetToSequence();
            findNextMarker();
            skipToNL = false;
        }
    }

    private void parseSequenceLevel() {
        ReadToken first = reader.getFirstReadToken();
        while (first.getKey() == '_' || first.getKey() == '%') {
            if (first.getKey() == '_') {
                if (hasEcho) {
                    error = MULTIPLE_ECHO_ERROR;
                    skipToNL = true;
                    break;
                }
                echo = first.getData16();
                hasEcho = true;
            } else if (first.getKey() == '%') {
                if (hasLocks) {
                    error = LOCKS_ERROR;
                    skipToNL = true;
                    break;
                }
                if (first.getDataSize() > ZcodeLocks.LOCK_BYTENUM) {
                    error = LOCKS_ERROR;
                    skipToNL = true;
                    break;
                }
                locks = ZcodeLockSet.from(first.blockIterator());
                hasLocks = true;
            }
            reader.flushFirstReadToken();
            first = reader.getFirstReadToken();
        }

        isAddressing = first.getKey() == '@';

        if (first.getKey() == '#') {
            if (!ZcodeTokenBuffer.isSequenceEndMarker(nextMarker)) {
                error = INTERNAL_ERROR;
                skipToNL = true;
            } else if (nextMarker != ZcodeTokenizer.NORMAL_SEQUENCE_END) {
                error = MARKER_ERROR;
                skipToNL = true;
            } else if (hasEcho || hasLocks) {
                error = COMMENT_WITH_SEQ_FIELDS_ERROR;
                skipToNL = true;
            } else {
                skipToNL = true;
            }
        }
    }

    private void resetToSequence() {
        locks = ZcodeLockSet.allLocked();
        atSeqStart = true;
        firstCommand = true;
        commandCanComplete = false;
        commandStarted = false;
        needsAction = false;

        prevMarker = 0;
        hasLocks = false;
        hasEcho = false;
        isAddressing = false;
        isFailed = false;
        isSkippingHandler = false;
        parenCounter = 0;
        skipToNL = false;
        findSeqEndMarker();
    }

    @Override
    public void setStatus(byte status) {

    }

    @Override
    public void setCommandStarted() {
        this.commandStarted = true;
    }

    @Override
    public byte getSeqEndMarker() {
        return seqEndMarker;
    }

    @Override
    public ZcodeLockSet getLocks() {
        return locks;
    }

    @Override
    public TokenReader getReader() {
        return reader;
    }

    @Override
    public boolean hasEcho() {
        return hasEcho;
    }

    @Override
    public int getEcho() {
        return echo;
    }

    @Override
    public void error() {
        skipToNL = true;
        error = OTHER_ERROR;
    }

    @Override
    public void setCommandCanComplete(final boolean b) {
        commandCanComplete = b;
    }

    @Override
    public void softFail() {
        isFailed = true;
        parenCounter = 0;
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
    public void clearFirstCommand() {
        firstCommand = false;
    }

    @Override
    public void errorSent() {
//        needSendError = false;
        error = NO_ERROR;
    }

    @Override
    public void seqEndSent() {
        needEndSeq = false;
    }

    @Override
    public void closeParenSent() {
        needCloseParen = false;
    }

    @Override
    public boolean commandCanComplete() {
        return commandCanComplete;
    }

    @Override
    public void setLocked(final boolean locked) {
        this.locked = locked;
    }

    @Override
    public boolean isLocked() {
        return locked;
    }

    @Override
    public void needsAction() {
        needsAction = true;
    }

    @Override
    public void clearNeedsAction() {
        needsAction = false;
    }

}
