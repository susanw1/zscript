package com.wittsfamily.rcode.javareceiver.parsing;

import com.wittsfamily.rcode.javareceiver.RCode;
import com.wittsfamily.rcode.javareceiver.RCodeParameters;
import com.wittsfamily.rcode.javareceiver.RCodeResponseStatus;
import com.wittsfamily.rcode.javareceiver.commands.RCodeCommand;
import com.wittsfamily.rcode.javareceiver.instreams.RCodeCommandInStream;

public class RCodeCommandSlot {
    private final RCode rcode;
    private final RCodeFieldMap map;
    private final RCodeBigField big;
    private final RCodeBigField bigBig;

    private char end = 0;
    private RCodeResponseStatus status = RCodeResponseStatus.OK;
    private String errorMessage = "";
    private boolean parsed = false;
    private boolean isBigBig = false;

    private RCodeCommand cmd = null;
    private boolean started = false;
    private boolean complete = false;

    public RCodeCommandSlot next = null;

    public RCodeCommandSlot(RCode rcode, RCodeParameters params, RCodeBigField bigBig) {
        this.rcode = rcode;
        this.map = new RCodeFieldMap(params);
        this.big = new RCodeBigField(params, false);
        this.bigBig = bigBig;
    }

    public void reset() {
        next = null;
        map.reset();
        big.reset();
        status = RCodeResponseStatus.OK;
        parsed = false;
        started = false;
        complete = false;
        if (isBigBig) {
            bigBig.reset();
            isBigBig = false;
        }
        cmd = null;
    }

    public void start() {
        started = true;
    }

    public boolean isStarted() {
        return started;
    }

    public void terminate() {
        end = '\n';
    }

    public boolean isComplete() {
        return complete;
    }

    public void setComplete(boolean complete) {
        this.complete = complete;
    }

    public RCodeFieldMap getFields() {
        return map;
    }

    public RCodeBigField getBigField() {
        if (isBigBig) {
            return bigBig;
        }
        return big;
    }

    private boolean parseHexField(RCodeCommandInStream in, char field) {
        while (in.hasNext() && in.peek() == '0') {
            in.read();
        }
        RCodeLookaheadStream l = in.getLookahead();
        char c = l.read();
        int lookahead = 0;
        while (isHex(c)) {
            c = l.read();
            lookahead++;
        }
        if (lookahead == 0) {
            return map.add(field, (byte) 0);
        } else if (lookahead == 1) {
            return map.add(field, getHex(in.read()));
        } else if (lookahead == 2) {
            return map.add(field, (byte) (getHex(in.read()) << 4 | getHex(in.read())));
        } else {
            byte current = 0;
            for (int extra = lookahead % (1 * 2); extra > 0; extra--) {
                current <<= 4;
                current += getHex(in.read());
                lookahead--;
            }
            if (current != 0 && !map.add(field, current)) {
                return false;
            }
            while (lookahead > 0) {
                for (int i = 0; i < 1 * 2; i++) {
                    current <<= 4;
                    current |= getHex(in.read());
                }
                lookahead -= 1 * 2;
                if (!map.add(field, current)) {
                    return false;
                }
            }
            return true;

        }
    }

    public RCodeCommand getCommand() {
        if (cmd == null) {
            cmd = rcode.getCommandFinder().findCommand(this);
        }
        return cmd;
    }

    private byte getHex(char c) {
        return (byte) (c >= 'a' ? c - 'a' + 10 : c - '0');
    }

    private boolean isHex(char c) {
        return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f');
    }

    private void failParse(RCodeCommandInStream in, RCodeResponseStatus errorStatus, String message) {
        status = errorStatus;
        errorMessage = message;
        in.close();
        end = '\n';
    }

    public boolean parseSingleCommand(RCodeCommandInStream in, RCodeCommandSequence sequence) {
        RCodeBigField target = big;
        in.open();
        reset();
        in.eatWhitespace();
        if (!in.hasNext()) {
            failParse(in, RCodeResponseStatus.PARSE_ERROR, "No command present");
            return false;
        }
        char c;
        parsed = true;
        while (true) {
            if (in.hasNext()) {
                in.eatWhitespace();
            }
            if (in.hasNext()) {
                c = in.read();
                if (c >= 'A' && c <= 'Z') {
                    if (map.has(c)) {
                        failParse(in, RCodeResponseStatus.PARSE_ERROR, "Same field appears twice");
                        return false;
                    } else if (!parseHexField(in, c)) {
                        failParse(in, RCodeResponseStatus.TOO_BIG, "Too many fields");
                        return false;
                    }
                } else if (c == '+') {
                    if (target.getLength() != 0) {
                        failParse(in, RCodeResponseStatus.PARSE_ERROR, "Multiple big fields");
                        return false;
                    }
                    target.setIsString(false);
                    byte d = getHex(in.peek());

                    while (in.hasNext() && isHex(in.peek())) {
                        d = getHex(in.read());
                        d <<= 4;
                        if (!in.hasNext() || !isHex(in.peek())) {
                            failParse(in, RCodeResponseStatus.PARSE_ERROR, "Big field odd digits");
                            return false;
                        }
                        d += getHex(in.read());
                        if (!target.addByteToBigField(d)) {
                            if (target == big) {
                                isBigBig = true;
                                target = bigBig;
                                big.copyTo(bigBig);
                                bigBig.addByteToBigField(d);
                            } else {
                                failParse(in, RCodeResponseStatus.TOO_BIG, "Big field too long");
                                return false;
                            }
                        }
                    }
                } else if (c == '"') {
                    if (target.getLength() != 0) {
                        failParse(in, RCodeResponseStatus.PARSE_ERROR, "Multiple big fields");
                        return false;
                    }
                    target.setIsString(true);
                    c = 0;
                    while (in.hasNext()) {
                        c = in.read();
                        if (c != '"') {
                            if (c == '\\') {
                                if (!in.hasNext()) {
                                    break;
                                }
                                c = in.read();
                                if (c == 'n') {
                                    c = '\n';
                                }
                            }
                            if (!target.addByteToBigField((byte) c)) {
                                if (target == big) {
                                    isBigBig = true;
                                    target = bigBig;
                                    target.setIsString(true);
                                    big.copyTo(bigBig);
                                    bigBig.addByteToBigField((byte) c);
                                } else {
                                    failParse(in, RCodeResponseStatus.TOO_BIG, "Big field too long");
                                    return false;
                                }
                            }
                        } else {
                            break;
                        }
                    }
                    if (c != '"') {
                        failParse(in, RCodeResponseStatus.PARSE_ERROR, "Command sequence ended before end of big string field");
                        return false;
                    }
                } else {
                    failParse(in, RCodeResponseStatus.PARSE_ERROR, "Unknown field marker with character code " + (int) c);
                    return false;
                }
            } else {
                in.close();
                end = in.read();
                if (getCommand() == null) {
                    failParse(in, RCodeResponseStatus.UNKNOWN_CMD, "Command not known");
                    return false;
                }
                return true;
            }
        }
    }

    public boolean isParsed() {
        return parsed;
    }

    public char getEnd() {
        return end;
    }

    public boolean isEndOfSequence() {
        return end == '\n';
    }

    public RCodeResponseStatus getStatus() {
        return status;
    }

    public String getErrorMessage() {
        return errorMessage;
    }

    public void fail(String errorMessage, RCodeResponseStatus status) {
        this.errorMessage = errorMessage;
        this.status = status;
    }
}
