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

    ZcodeOutputStream out;
    Zcode             zcode;
    ZcodeTokenBuffer  buffer;
    ZcodeLockSet      locks;

    int     sequenceType      = UNKNOWN_SEQUENCE;
    boolean hasFoundSeperator = false;
    boolean locked            = false;
    boolean outLocked         = false;
    boolean skipToNewline     = false;
    byte    failStatus        = 0;

    public void moveAlong() {
        if (failStatus != 0) {
            fail(failStatus);
        }
        if (!hasFoundSeperator && buffer.hasNewState()) {
            checkForSeperator();
            buffer.clearNewState();
        }
        if (skipToNewline) {
            skipToNL();
        } else if (sequenceType == UNKNOWN_SEQUENCE) {
            parseSeqBegin();
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
            addrSeq.parse(this);
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
        failStatus = 0;
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
        if (lockOut()) {
            out.writeStatus(code);
            out.close();
            resetSequence();
        } else {
            failStatus = code;
        }
    }

    private void unlock() {
        if (locked) {
            zcode.unlock(locks);
            locked = false;
        }
        if (outLocked) {
            out.unlock();
            outLocked = false;
        }
    }

    public boolean lockOut() {
        outLocked = outLocked || out.lock();
        return outLocked;
    }

    public boolean lockLockset() {
        locked = locked || zcode.lock(locks);
        return locked;
    }

    public boolean canLockOut() {
        return outLocked || out.lock();
    }

    public boolean canLockLockset() {
        return locked || zcode.canLock(locks);
    }

    public boolean canLock() {
        return (locked || zcode.canLock(locks)) && (outLocked || !out.isLocked());
    }

    public boolean lock() {
        if (outLocked || !out.isLocked()) {
            if (locked || zcode.lock(locks)) {
                return outLocked || out.lock();
            }
        }
        return false;
    }

    public boolean canLockExec() {
        if (sequenceType == COMMAND_SEQUENCE) {
            return canLock();
        } else if (sequenceType == ADDRESSED_SEQUENCE) {
            return canLockLockset();
        } else {
            return canLockOut();
        }
    }

    public boolean lockExec() {
        if (sequenceType == COMMAND_SEQUENCE) {
            return lock();
        } else if (sequenceType == ADDRESSED_SEQUENCE) {
            return lockLockset();
        } else {
            return lockOut();
        }
    }

    public boolean commandReady() {
        return hasFoundSeperator && sequenceType == COMMAND_SEQUENCE;
    }

    public boolean addressingReady() {
        return hasFoundSeperator && sequenceType == ADDRESSED_SEQUENCE;
    }

    public boolean needsRunning() {
        return (sequenceType == COMMAND_SEQUENCE && hasFoundSeperator && cmdSeq.needsRunning())
                || (sequenceType == ADDRESSED_SEQUENCE && hasFoundSeperator && addrSeq.needsRunning());
    }

    public void run() {
        if (sequenceType == ADDRESSED_SEQUENCE) {
            addrSeq.run(this);
        } else if (sequenceType == COMMAND_SEQUENCE) {
            if (cmdSeq.checkParse(this)) {
                cmdSeq.run(this);
            }
        }
    }
}
