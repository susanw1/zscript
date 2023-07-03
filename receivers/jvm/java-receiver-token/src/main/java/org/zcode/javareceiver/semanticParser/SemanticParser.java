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
    public static final int NO_ERROR                 = 0;
    public static final int INTERNAL_ERROR           = 1;
    public static final int MARKER_ERROR             = 2;
    public static final int MULTIPLE_ECHO_ERROR      = 3;
    public static final int MULTIPLE_LOCKS_ERROR     = 4;
    public static final int COMMENT_WITH_STUFF_ERROR = 5;
    public static final int OTHER_ERROR              = 6;

    private final TokenReader reader;

    private ZcodeLockSet locks    = null;
    private boolean      hasLocks = false;
    private int          echo     = 0;
    private boolean      hasEcho  = false;

    private byte    nextMarker     = 0;
    private boolean haveNextMarker = false;

    private byte    seqEndMarker     = 0;
    private boolean haveSeqEndMarker = false;

    private boolean atSeqStart = true;

    private boolean isAddressing = false;

    private boolean complete     = false;
    private boolean started      = false;
    private boolean needsAction  = false;
    private boolean firstCommand = false;

    private boolean isFailed          = false;
    private boolean isSkippingHandler = false;
    private int     bracketCounter    = 0;

    private int error = NO_ERROR;

    private boolean skipToNL      = false;
    private boolean needSendError = false;

    public SemanticParser(TokenReader reader) {
        this.reader = reader;
    }

    public ZcodeAction getAction() {
        if (needSendError) {
            return ZcodeAction.error(this, error);
        }
        if (complete) {
            complete = false;
            if (isAddressing) {
                reader.flushFirstReadToken();
                resetToSequence();
            } else {
                byte marker = reader.getFirstReadToken().getKey();
                reader.flushFirstReadToken();
                flowControl(marker);
            }
            findNextMarker();
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
                    skipToMarker();
                    return ZcodeAction.noAction(this);
                } else {
                    return ZcodeAction.runCommand(this);
                }
            }
        }
    }

    private void flowControl(byte marker) {
        if (ZcodeTokenBuffer.isSequenceEndMarker(marker)) {
            resetToSequence();
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
        ZcodeTokenBufferFlags flags = new ZcodeTokenBufferFlags(); // TODO: get this properly
        flags.setMarkerWritten();
        flags.setSeqMarkerWritten();
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
        error = NO_ERROR;
        skipToNL = false;
        needSendError = false;
        findSeqEndMarker();
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

    public void fail() {
        skipToNL = true;
        error = OTHER_ERROR;
    }

    public void setComplete() {
        complete = true;
    }
}
