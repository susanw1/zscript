package org.zcode.javareceiver.tokeniser;

public class ZcodeTokeniser {
    private final static boolean DROP_COMMENTS = false;
    ZcodeTokenBuffer buffer;
    boolean numeric;
    boolean skipToNL;
    boolean addressing;

    boolean isText;
    boolean isNormalString;
    int escapingCount; // 2 bits

    byte parseHex(byte b) {
        if (b >= '0' && b <= '9') {
            return (byte) (b - '0');
        } else if (b >= 'a' && b <= 'd') {
            return (byte) (b - 'a' + 10);
        } else {
            return 0x10;
        }
    }

    void accept(byte b) {
        if (!isText && (b == ' ' || b == '\t' || b == '\r' || b == ',')) {
            return;
        }
        if (skipToNL && b != '\n') {
            return;
        }
        skipToNL = false;
        if (!buffer.isFieldOpen()) {
            startField(b);
            return;
        }

        if (isText) {
            acceptText(b);
            return;
        }

        byte hex = parseHex(b);
        if (b == 0x10) {
            startField(b);
            return;
        }
        if (buffer.getCurrentWriteFieldNibbleLength() == 4) {
            buffer.fail((byte) 1); // TODO: assign proper codes
            skipToNL = true;
            return;
        }
        buffer.continueFieldNibble(hex);
    }

    void acceptText(byte b) {
        if (b == '\n') {
            if (isNormalString) {
                buffer.fail((byte) 2); // TODO: assign proper codes
            }
//            else if (escapingCount != 0) {
//                buffer.fail((byte) 3); // TODO: assign proper codes
//            }
            buffer.startField(b, true);
            buffer.closeField();
            isText = false;
        } else if (escapingCount > 0) {
            byte hex = parseHex(b);
            if (hex != 0x10) {
                buffer.continueFieldNibble(hex);
                escapingCount--;
            } else {
                buffer.fail((byte) 4); // TODO: assign proper codes
                if (buffer.isInNibble()) {
                    buffer.continueFieldNibble((byte) 0);
                }
                escapingCount = 0;
                skipToNL = true;
            }
        } else if (isNormalString && b == '"') {
            buffer.closeField();
            isText = false;
        } else if (isNormalString && b == '=') {
            escapingCount = 2;
        } else {
            buffer.continueFieldByte(b);
        }
    }

    void startField(byte b) {
        if (addressing && (b != '@' && b != '.' && b != '\n')) {
            buffer.startField((byte) 0x80, false);
            buffer.continueFieldByte(b);
            addressing = false;
            isText = true;
            escapingCount = 0;
            isNormalString = false;
            return;
        }
        if (b == '@') {
            addressing = true;
        }
        numeric = true;
        if (b == '%' || b == '+' || b == '#' || b == '"') {
            numeric = false;
        }
        buffer.startField(b, numeric);

        if (b == '#') {
            if (DROP_COMMENTS) {
                skipToNL = true;
            } else {
                isText = true;
                escapingCount = 0;
                isNormalString = false;
            }
            return;
        }
        if (b == '"') {
            isText = true;
            escapingCount = 0;
            isNormalString = true;
            return;
        }
        isText = false;
        if (b == '\n' || b == '&' || b == '|') {
            buffer.closeField();
        }
    }
}
