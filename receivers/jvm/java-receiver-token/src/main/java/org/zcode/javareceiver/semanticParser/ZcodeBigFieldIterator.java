package org.zcode.javareceiver.semanticParser;

import org.zcode.javareceiver.tokenizer.Zchars;
import org.zcode.javareceiver.tokenizer.ZcodeTokenBuffer;
import org.zcode.javareceiver.tokenizer.ZcodeTokenDataIterator;
import org.zcode.javareceiver.tokenizer.ZcodeTokenIterator;

public class ZcodeBigFieldIterator {
    private final ZcodeTokenIterator     tokens      = new ZcodeTokenIterator();
    private final ZcodeTokenDataIterator data        = new ZcodeTokenDataIterator();
    private boolean                      hasNextData = false;
    private boolean                      hasData     = false;

    public void setup(ZcodeTokenBuffer buffer) {
        buffer.setIterator(tokens);
        while (tokens.isValid() && !Zchars.isSeparator(tokens.getCurrentKey())) {
            if (tokens.getCurrentKey() == '+' || tokens.getCurrentKey() == '"') {
                hasNextData = true;
                break;
            }
            tokens.next();
        }
        if (tokens.isValid() && !Zchars.isSeparator(tokens.getCurrentKey())) {
            setDataToNext();
            hasData = true;
        }
    }

    private void setDataToNext() {
        tokens.setTokenDataIterator(data);
        if (tokens.isValid()) {
            tokens.next();
            while (tokens.isValid() && !Zchars.isSeparator(tokens.getCurrentKey())) {
                if ((tokens.getCurrentKey() == '+' || tokens.getCurrentKey() == '"') && tokens.hasData()) {
                    hasNextData = true;
                    return;
                }
                tokens.next();
            }
        }
        hasNextData = false;
    }

    boolean hasNext() {
        return hasData && data.hasNext() || hasNextData;
    }

    byte takeNext() {
        return data.next();
    }

    byte[] takeContiguous() {
        return data.takeContiguous();
    }
}
