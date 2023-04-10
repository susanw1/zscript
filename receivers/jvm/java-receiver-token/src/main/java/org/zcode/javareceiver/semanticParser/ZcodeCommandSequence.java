package org.zcode.javareceiver.semanticParser;

import org.zcode.javareceiver.core.ZcodeStatus;
import org.zcode.javareceiver.tokenizer.Zchars;
import org.zcode.javareceiver.tokenizer.ZcodeTokenIterator;

public class ZcodeCommandSequence {
    int     echoField;
    boolean hasEcho       = false;
    boolean isSkippingOr  = false;
    boolean hasMildFailed = false;
    boolean canRun        = false;

    public void setEcho(int numericValue) {
        echoField = numericValue;
        hasEcho = true;
    }

    public void reset() {
        hasEcho = false;
        isSkippingOr = false;
        hasMildFailed = false;
    }

    public void run(ZcodeReceiverSequence seq) {
        // TODO Auto-generated method stub
    }

    public boolean needsRunning() {
        // TODO Auto-generated method stub
        return false;
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
        }
        if (!iterator.isValid()) {
            throw new IllegalStateException("Command run before complete");
        }
        return true;
    }

}
