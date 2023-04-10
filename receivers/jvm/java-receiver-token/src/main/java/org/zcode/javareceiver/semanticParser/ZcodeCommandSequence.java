package org.zcode.javareceiver.semanticParser;

import org.zcode.javareceiver.core.ZcodeOutputStream;
import org.zcode.javareceiver.core.ZcodeStatus;
import org.zcode.javareceiver.tokenizer.Zchars;
import org.zcode.javareceiver.tokenizer.ZcodeTokenIterator;

public class ZcodeCommandSequence implements ZcodeCommandCallbackAccess {
    int     echoField     = 0;
    boolean hasEcho       = false;
    boolean isSkippingOr  = false;
    boolean hasMildFailed = false;
    boolean needsAction   = false;
    boolean hasStarted    = false;
    boolean isComplete    = false;

    int bracketCounter = 0;

    public void setEcho(int numericValue) {
        echoField = numericValue;
        hasEcho = true;
    }

    public void reset() {
        hasEcho = false;
        isSkippingOr = false;
        hasMildFailed = false;
    }

    public void parse(ZcodeReceiverSequence seq) {
        if (!isComplete) {
            return;
        }
        if (seq.hasFoundSeperator) {
            ZcodeTokenIterator iterator = new ZcodeTokenIterator();
            seq.buffer.setIterator(iterator);
            if (iterator.getCurrentKey() == '\n') {
                seq.skipToNL();
                return;
            }
            isComplete = false;
            seq.buffer.setIterator(iterator);
            while (iterator.isValid()) {
                byte key = iterator.getCurrentKey();
                if (key == '\n') {
                    seq.skipToNL();
                    return;
                }
                if (key == '&') {
                    if (!hasMildFailed && !isSkippingOr) {
                        iterator.next();
                        iterator.moveReadStart();
                        return;
                    }
                } else if (key == '|') {
                    if (hasMildFailed && bracketCounter == 0) {
                        iterator.next();
                        iterator.moveReadStart();
                        return;
                    }
                    if (!hasMildFailed) {
                        isSkippingOr = true;
                    }
                } else if (key == '(') {
                    if (hasMildFailed || isSkippingOr) {
                        bracketCounter++;
                    } else {
                        iterator.next();
                        iterator.moveReadStart();
                        return;
                    }
                } else if (key == ')') {
                    if (hasMildFailed) {
                        if (bracketCounter > 0) {
                            bracketCounter--;
                        }
                    } else if (isSkippingOr) {
                        if (bracketCounter == 0) {
                            iterator.next();
                            iterator.moveReadStart();
                            return;
                        } else {
                            bracketCounter--;
                        }
                    } else {
                        iterator.next();
                        iterator.moveReadStart();
                        return;
                    }
                }
                iterator.next();
            }
        }
    }

    public void run(ZcodeReceiverSequence seq, ZcodeOutputStream out) {
        if (hasEcho) {
            out.writeField((byte) '_', echoField);
            hasEcho = false;
        }
        hasStarted = true;
        needsAction = false;
        isComplete = true;
        ZcodeCommandRunningSystem runner = new ZcodeCommandRunningSystem(this, seq.buffer, out);

        // TODO: run

        byte status = runner.getStatus();
        if (status != 0) {
            isComplete = true;
            if (status <= 0x10) {
                seq.skipToNL();
                return;
            } else {
                hasMildFailed = true;
                bracketCounter = 0;
            }
        }
        if (isComplete) {
            ZcodeTokenIterator iterator = new ZcodeTokenIterator();
            seq.buffer.setIterator(iterator);
            while (iterator.isValid() && !Zchars.isSeparator(iterator.getCurrentKey())) {
                iterator.next();
            }
            iterator.moveReadStart();
        }
    }

    public boolean needsRunning() {
        return needsAction;
    }

    public boolean checkParse(ZcodeReceiverSequence seq) {
        ZcodeTokenIterator iterator = new ZcodeTokenIterator();
        seq.buffer.setIterator(iterator);
        while (iterator.isValid() && !Zchars.isSeparator(iterator.getCurrentKey())) {
            byte key = iterator.getCurrentKey();
            if (!Zchars.isCommandKey(key)) {
                seq.fail(ZcodeStatus.ILLEGAL_CHARACTER);
                return false;
            }
            iterator.next();
        }
        if (!iterator.isValid()) {
            throw new IllegalStateException("Command run before complete");
        }
        return true;
    }

    @Override
    public void setNeedsAttention() {
        needsAction = true;
    }

}
