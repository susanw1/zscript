package org.zcode.javareceiver.semanticParser;

import java.util.Optional;

import org.zcode.javareceiver.core.ZcodeLockSet;
import org.zcode.javareceiver.tokenizer.TokenBufferIterator;
import org.zcode.javareceiver.tokenizer.ZcodeTokenBuffer;
import org.zcode.javareceiver.tokenizer.ZcodeTokenBuffer.TokenReader;
import org.zcode.javareceiver.tokenizer.ZcodeTokenBuffer.TokenReader.ReadToken;
import org.zcode.javareceiver.tokenizer.ZcodeTokenBufferFlags;
import org.zcode.javareceiver.tokenizer.ZcodeTokenizer;

public class SemanticParser {
    // 16 booleans, 5 uint8_t, 1 uint16_t = 9 bytes of status

    public static final byte NO_ERROR                 = 0;
    public static final byte INTERNAL_ERROR           = 1;
    public static final byte MARKER_ERROR             = 2;
    public static final byte MULTIPLE_ECHO_ERROR      = 3;
    public static final byte MULTIPLE_LOCKS_ERROR     = 4;
    public static final byte COMMENT_WITH_STUFF_ERROR = 5;
    public static final byte OTHER_ERROR              = 6;

    private final TokenReader reader;

    private ZcodeLockSet locks    = null;
    private boolean      hasLocks = false;
    private int          echo     = 0;    // 16 bit
    private boolean      hasEcho  = false;

    private byte    nextMarker     = 0;
    private boolean haveNextMarker = false;

    private byte prevMarker = 0;

    private byte    seqEndMarker     = 0;
    private boolean haveSeqEndMarker = false;

    private boolean atSeqStart = true;

    private boolean isAddressing = false;

    private boolean complete     = false;
    private boolean started      = false;
    private boolean needsAction  = false;
    private boolean firstCommand = true;

    private boolean isFailed          = false;
    private boolean isSkippingHandler = false;
    private byte    bracketCounter    = 0;

    private byte error = NO_ERROR;

    private boolean skipToNL      = false;
    private boolean needSendError = false;
    private boolean needEndSeq    = false;

    private boolean activated = false;

    public SemanticParser(TokenReader reader) {
        this.reader = reader;
    }

    public ZcodeAction getAction() {
        if (needSendError) {
            return ZcodeAction.error(this, error);
        }

        if (complete) {
            complete = false;
            started = false;
            skipToMarker();
            if (isAddressing) {
                reader.flushFirstReadToken();
                resetToSequence();
            } else {
                byte marker = reader.getFirstReadToken().getKey();
                reader.flushFirstReadToken();
                flowControl(marker);
            }
            prevMarker = nextMarker;
            findNextMarker();
        }
        if (needEndSeq) {
            return ZcodeAction.endSequence(this);
        }
        dealWithFlags();
        if (!haveNextMarker) {
            return ZcodeAction.noAction(this);
        }
        if (atSeqStart) {
            atSeqStart = false;
            parseSequenceLevel();
            if (skipToNL && error != NO_ERROR) {
                skipToSeqEnd();
                needSendError = true;
                return ZcodeAction.error(this, error);
            }
        }
        if (skipToNL) {
            skipToSeqEnd();
            needSendError = false;
            error = NO_ERROR;
            return ZcodeAction.noAction(this);
        }
        if (isAddressing) {
            if (error != NO_ERROR) {
                needSendError = true;
                return ZcodeAction.error(this, error);
            }
            if (started) {
                if (needsAction) {
                    return ZcodeAction.addressing(this);
                } else {
                    return ZcodeAction.noAction(this);
                }
            } else {
                return ZcodeAction.addressing(this);
            }
        } else {
            if (started) {
                if (needsAction) {
                    return ZcodeAction.commandMoveAlong(this);
                } else {
                    return ZcodeAction.noAction(this);
                }
            } else if (error != NO_ERROR) {
                skipToNL = true;
                skipToSeqEnd();
                needSendError = true;
                return ZcodeAction.error(this, error);
            } else if (firstCommand) {
                return ZcodeAction.firstCommand(this);
            } else {
                if (isSkippingHandler || isFailed) {
                    complete = true;
                    return ZcodeAction.noAction(this);
                } else {
                    return ZcodeAction.runCommand(this, prevMarker);
                }
            }
        }
    }

    private void flowControl(byte marker) {
        if (ZcodeTokenBuffer.isSequenceEndMarker(marker)) {
            resetToSequence();
            needEndSeq = true;
        } else {
            if (marker == ZcodeTokenizer.CMD_END_ORELSE) {
                if (isFailed) {
                    isFailed = false;
                } else {
                    isSkippingHandler = true;
                }
            }
        }
    }

    private void dealWithFlags() {
        ZcodeTokenBufferFlags flags = reader.getFlags();
        if (flags.isMarkerWritten()) {
            flags.clearMarkerWritten();
            if (!haveNextMarker) {
                findNextMarker();
            }
        }
        if (flags.isSeqMarkerWritten()) {
            flags.clearSeqMarkerWritten();
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
        TokenBufferIterator it    = reader.iterator();
        Optional<ReadToken> token = it.next();
        while (token.isPresent() && !token.get().isMarker()) {
            token = it.next();
        }
        if (token.isPresent()) {
            nextMarker = token.get().getKey();
            haveNextMarker = true;
        } else {
            haveNextMarker = false;
        }
    }

    private void findSeqEndMarker() {
        TokenBufferIterator it    = reader.iterator();
        Optional<ReadToken> token = it.next();
        while (token.isPresent() && !ZcodeTokenBuffer.isSequenceEndMarker(token.get().getKey())) {
            token = it.next();
        }
        if (token.isPresent()) {
            seqEndMarker = token.get().getKey();
            if (token.get().getKey() != ZcodeTokenizer.NORMAL_SEQUENCE_END) {
                error = MARKER_ERROR;
            }
            haveSeqEndMarker = true;
        } else {
            haveSeqEndMarker = false;
        }
    }

    private void skipToMarker() {
        TokenBufferIterator it = reader.iterator();
        for (Optional<ReadToken> token = it.next(); token.isPresent() && !token.get().isMarker(); token = it.next()) {
            it.flushBuffer();
        }
    }

    private void skipToSeqEnd() {
        byte marker = reader.getFirstReadToken().getKey();
        while (reader.hasReadToken() && !ZcodeTokenBuffer.isSequenceEndMarker(marker)) {
            skipToMarker();
            marker = reader.getFirstReadToken().getKey();
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
                    error = MULTIPLE_LOCKS_ERROR;
                    skipToNL = true;
                    break;
                }
                locks = ZcodeLockSet.from(first);
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
                error = COMMENT_WITH_STUFF_ERROR;
                skipToNL = true;
            } else {
                skipToNL = true;
            }
        }
    }

    private void resetToSequence() {
        locks = ZcodeLockSet.allLocked();
        prevMarker = 0;
        hasLocks = false;
        hasEcho = false;
        atSeqStart = true;
        isAddressing = false;
        complete = false;
        started = false;
        needsAction = false;
        firstCommand = true;
        isFailed = false;
        isSkippingHandler = false;
        bracketCounter = 0;
        skipToNL = false;
        findSeqEndMarker();
    }

    public void setStarted() {
        this.started = true;
    }

    public byte getSeqEndMarker() {
        return seqEndMarker;
    }

    public ZcodeLockSet getLocks() {
        return locks;
    }

    public TokenReader getReader() {
        return reader;
    }

    public boolean hasEcho() {
        return hasEcho;
    }

    public int getEcho() {
        return echo;
    }

    public void error() {
        skipToNL = true;
        error = OTHER_ERROR;
    }

    public void setComplete(boolean b) {
        complete = b;
    }

    public void softFail() {
        isFailed = true;
    }

    public boolean isActivated() {
        return activated;
    }

    public void activate() {
        activated = true;
    }

    public void clearFirstCommand() {
        firstCommand = false;
    }

    public void errorSent() {
        needSendError = false;
    }

    public void seqEndSent() {
        needEndSeq = false;
    }

    public boolean isComplete() {
        return complete;
    }

}
