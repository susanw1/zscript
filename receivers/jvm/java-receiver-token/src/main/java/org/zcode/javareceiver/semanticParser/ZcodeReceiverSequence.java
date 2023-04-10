package org.zcode.javareceiver.semanticParser;

import org.zcode.javareceiver.core.Zcode;
import org.zcode.javareceiver.core.ZcodeLockSet;
import org.zcode.javareceiver.core.ZcodeOutputStream;
import org.zcode.javareceiver.tokenizer.Zchars;
import org.zcode.javareceiver.tokenizer.ZcodeTokenBuffer;
import org.zcode.javareceiver.tokenizer.ZcodeTokenDataIterator;
import org.zcode.javareceiver.tokenizer.ZcodeTokenIterator;

public class ZcodeReceiverSequence {
    private static final int UNKNOWN_SEQUENCE   = 0;
    private static final int COMMAND_SEQUENCE   = 1;
    private static final int ADDRESSED_SEQUENCE = 2;

    private final ZcodeAddressingSequence addrSeq = new ZcodeAddressingSequence();
    private final ZcodeCommandSequence    cmdSeq  = new ZcodeCommandSequence();

    Zcode            zcode;
    ZcodeTokenBuffer buffer;
    ZcodeLockSet     locks;

    int     sequenceType      = UNKNOWN_SEQUENCE;
    boolean hasFoundSeperator = false;
    boolean locked            = false;
    boolean skipToNewline     = false;
    byte    failStatus        = 0;

    // TODO: add hasFoundNewLine to check for errors as well

    public void moveAlong() {
        if (!hasFoundSeperator && buffer.hasNewState()) {
            checkForSeperator();
            buffer.clearNewState();
        }
        if (skipToNewline) {
            skipToNL();
        } else if (failStatus != 0) {
            return;
        } else if (sequenceType == UNKNOWN_SEQUENCE) {
            parseSeqBegin();
        } else if (sequenceType == COMMAND_SEQUENCE) {
            cmdSeq.parse(this);
        }
    }

    private void parseSeqBegin() {
        ZcodeTokenIterator iterator = new ZcodeTokenIterator();
        buffer.setIterator(iterator);
        if (!iterator.isValid()) {
            return;
        }
        byte key = iterator.getCurrentKey();
        switch (key) {
        case '_':
            sequenceType = COMMAND_SEQUENCE;
            cmdSeq.setEcho(iterator.getNumericValue());
            iterator.next();
            iterator.moveReadStart();
            break;
        case '%':
            ZcodeTokenDataIterator dataIt = new ZcodeTokenDataIterator();
            iterator.setTokenDataIterator(dataIt);
            locks.set(dataIt);
            iterator.next();
            iterator.moveReadStart();
            break;
        case '#':
            // TODO: do something with comments.
            skipToNL();
            break;
        case '\n':
            iterator.next();
            iterator.moveReadStart();
            resetSequence();
            break;
        case '@':
            sequenceType = ADDRESSED_SEQUENCE;
            break;
        default:
            sequenceType = COMMAND_SEQUENCE;
            cmdSeq.parse(this);
            break;
        }
    }

    public void skipToNL() {
        unlock();
        skipToNewline = true;
        ZcodeTokenIterator iterator = new ZcodeTokenIterator();
        buffer.setIterator(iterator);
        while (iterator.isValid() && iterator.getCurrentKey() != '\n') {
            iterator.next();
        }
        if (iterator.isValid()) {
            iterator.next();
            resetSequence();
        }
        iterator.moveReadStart();
    }

    private void resetSequence() {
        unlock();
        addrSeq.reset();
        cmdSeq.reset();
        locks.setAll();
        sequenceType = UNKNOWN_SEQUENCE;
        skipToNewline = false;
        checkForSeperator();
    }

    private void checkForSeperator() {
        ZcodeTokenIterator iterator = new ZcodeTokenIterator();
        buffer.setIterator(iterator);
        while (iterator.isValid()) {
            if (Zchars.isSeparator(iterator.getCurrentKey())) {
                if (iterator.getCurrentKey() != Zchars.Z_NEWLINE && sequenceType == ADDRESSED_SEQUENCE) {
                    throw new IllegalStateException("Can't have non-newline seperators with addressing");
                }
                hasFoundSeperator = true;
                return;
            } else if ((iterator.getCurrentKey() & 0xF0) == 0xF0) {
                fail(mapTokenFails(iterator.getCurrentKey()));
                return;
            }
            iterator.next();
        }
    }

    private byte mapTokenFails(byte code) {
        // TODO: add error mapping
    }

    public void fail(byte code) {
        failStatus = code;
        skipToNL();
    }

    private void unlock() {
        if (locked) {
            zcode.unlock(locks);
            locked = false;
        }
    }

    public boolean canLock() {
        return locked || zcode.canLock(locks);
    }

    public boolean lock() {
        if (locked || zcode.lock(locks)) {
            return true;
        }
        return false;
    }

    public boolean needsOutput() {
        return sequenceType != ADDRESSED_SEQUENCE;
    }

    public boolean needsRunning() {
        return failStatus != 0 || (sequenceType == COMMAND_SEQUENCE && hasFoundSeperator && cmdSeq.needsRunning())
                || (sequenceType == ADDRESSED_SEQUENCE && hasFoundSeperator && addrSeq.needsRunning());
    }

    public boolean isFinished() {
        return sequenceType == UNKNOWN_SEQUENCE;
    }

    public void run(ZcodeOutputStream out) {
        if (failStatus != 0) {
            out.writeStatus(failStatus);
            failStatus = 0;
        } else if (sequenceType == ADDRESSED_SEQUENCE) {
            addrSeq.run(this);
        } else if (sequenceType == COMMAND_SEQUENCE) {
            if (cmdSeq.checkParse(this)) {
                cmdSeq.run(this, out);
            }
        }
    }
}
