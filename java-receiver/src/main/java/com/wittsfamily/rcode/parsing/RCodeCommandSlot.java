package com.wittsfamily.rcode.parsing;

import com.wittsfamily.rcode.RCode;
import com.wittsfamily.rcode.RCodeParameters;
import com.wittsfamily.rcode.RCodeResponseStatus;
import com.wittsfamily.rcode.commands.RCodeCommand;

public class RCodeCommandSlot {
    private final RCode rcode;
    private final RCodeFieldMap map;
    private final RCodeBigField big;
    private final RCodeBigField bigBig;
    private char end = 0;
    private RCodeResponseStatus status = RCodeResponseStatus.OK;
    private String errorMessage = "";
    private boolean parsed = false;
    private boolean complete = false;
    private boolean started = false;
    private boolean isBigBig = false;
    private RCodeCommand cmd = null;
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

    private void eatWhitespace(RCodeInStream in) {
        char c = in.peek();
        while (in.hasNext() && (c == ' ' || c == '\t' || c == '\r')) {
            c = in.read();
        }
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

    private boolean parseHexField(RCodeInStream in, char field) {
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

    public boolean parseSingleCommand(RCodeInStream in, RCodeCommandSequence sequence) {
        RCodeBigField target = big;
        in.openCommand();
        reset();
        char c;
        eatWhitespace(in);
        if (in.peek() == '*') {
            in.read();
            if (sequence.peekFirst() == null) {
                sequence.setBroadcast();
            } else {
                status = RCodeResponseStatus.PARSE_ERROR;
                errorMessage = "* only valid on first command of sequence";
                in.closeCommand();
                end = '\n';
                return false;
            }
        }
        if (in.peek() == '%') {
            in.read();
            if (sequence.peekFirst() == null) {
                sequence.setParallel();
            } else {
                status = RCodeResponseStatus.PARSE_ERROR;
                errorMessage = "% only valid on first command of sequence";
                in.closeCommand();
                end = '\n';
                return false;
            }
        }
        parsed = true;
        while (true) {
            if (in.hasNext()) {
                eatWhitespace(in);
            }
            if (in.hasNext()) {
                c = in.read();
                if (c >= 'A' && c <= 'Z') {
                    if (!parseHexField(in, c)) {
                        status = RCodeResponseStatus.TOO_BIG;
                        errorMessage = "Too many fields";
                        in.closeCommand();
                        end = '\n';
                        return false;
                    }
                } else if (c == '+') {
                    if (target.getLength() != 0) {
                        status = RCodeResponseStatus.PARSE_ERROR;
                        errorMessage = "Multiple big fields";
                        in.closeCommand();
                        end = '\n';
                        return false;
                    }
                    target.setIsString(false);
                    byte d = getHex(in.peek());

                    while (in.hasNext() && isHex(in.peek())) {
                        in.read();
                        d <<= 4;
                        if (!in.hasNext() || !isHex(in.peek())) {
                            status = RCodeResponseStatus.PARSE_ERROR;
                            errorMessage = "Big field odd digits";
                            in.closeCommand();
                            end = '\n';
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
                                status = RCodeResponseStatus.TOO_BIG;
                                errorMessage = "Big field too long";
                                in.closeCommand();
                                end = '\n';
                                return false;
                            }
                        }
                    }
                } else if (c == '"') {
                    if (target.getLength() != 0) {
                        status = RCodeResponseStatus.PARSE_ERROR;
                        errorMessage = "Multiple big fields";
                        in.closeCommand();
                        end = '\n';
                        return false;
                    }
                    target.setIsString(true);
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
                                    status = RCodeResponseStatus.TOO_BIG;
                                    errorMessage = "Big field too long";
                                    in.closeCommand();
                                    end = '\n';
                                    return false;
                                }
                            }
                        } else {
                            break;
                        }
                    }
                    if (c != '"') {
                        status = RCodeResponseStatus.PARSE_ERROR;
                        errorMessage = "Command sequence ended before end of big string field";
                        in.closeCommand();
                        end = '\n';
                        return false;
                    }
                } else {
                    status = RCodeResponseStatus.PARSE_ERROR;
                    errorMessage = "Unknown field marker: " + c;
                    in.closeCommand();
                    end = '\n';
                    return false;
                }
            } else {
                in.closeCommand();
                end = in.read();
                if (getCommand() == null) {
                    status = RCodeResponseStatus.UNKNOWN_CMD;
                    errorMessage = "Command not known";
                    in.closeCommand();
                    end = '\n';
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

    public RCodeResponseStatus getStatus() {
        return status;
    }

    public String getErrorMessage() {
        return errorMessage;
    }
}
