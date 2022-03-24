package org.zcode.javareceiver.parsing;

import org.zcode.javareceiver.Zchars;
import org.zcode.javareceiver.Zcode;
import org.zcode.javareceiver.ZcodeParameters;
import org.zcode.javareceiver.ZcodeResponseStatus;
import org.zcode.javareceiver.commands.ZcodeCommand;
import org.zcode.javareceiver.instreams.ZcodeCommandInStream;
import org.zcode.javareceiver.instreams.ZcodeLookaheadStream;

public class ZcodeCommandSlot {
    private final Zcode         zcode;
    private final ZcodeFieldMap map;
    private final ZcodeBigField big;
    private final ZcodeBigField huge;

    private ZcodeResponseStatus status = ZcodeResponseStatus.OK;

    private char    end          = 0;
    private String  errorMessage = "";
    private boolean parsed       = false;
    private boolean isHuge       = false;

    private ZcodeCommand cmd            = null;
    private boolean      started        = false;
    private boolean      complete       = false;
    private boolean      needsMoveAlong = false;

    public ZcodeCommandSlot next = null;

    public ZcodeCommandSlot(final Zcode zcode, final ZcodeParameters params, final ZcodeBigField huge) {
        this.zcode = zcode;
        this.map = new ZcodeFieldMap(params);
        this.big = new ZcodeBigField(params, false);
        this.huge = huge;
    }

    public void reset() {
        next = null;
        map.reset();
        big.reset();
        status = ZcodeResponseStatus.OK;
        parsed = false;
        started = false;
        complete = false;
        needsMoveAlong = false;
        if (isHuge) {
            huge.reset();
            isHuge = false;
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
        end = Zchars.EOL_SYMBOL.ch;
    }

    public boolean needsMoveAlong() {
        return needsMoveAlong;
    }

    public void setNeedsMoveAlong(final boolean needsMoveAlong) {
        this.needsMoveAlong = needsMoveAlong;
    }

    public boolean isComplete() {
        return complete;
    }

    public void setComplete(final boolean complete) {
        this.complete = complete;
    }

    public ZcodeFieldMap getFields() {
        return map;
    }

    public ZcodeBigField getBigField() {
        if (isHuge) {
            return huge;
        }
        return big;
    }

    private boolean parseHexField(final ZcodeCommandInStream in, final char field) {
        while (in.hasNext() && in.peek() == '0') {
            in.read();
        }
        final ZcodeLookaheadStream l = in.getLookahead();

        char c         = l.read();
        int  lookahead = 0;
        
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

    public ZcodeCommand getCommand() {
        if (cmd == null) {
            cmd = zcode.getCommandFinder().findCommand(this);
        }
        return cmd;
    }

    private byte getHex(final char c) {
        return (byte) (c >= 'a' ? c - 'a' + 10 : c - '0');
    }

    private boolean isHex(final char c) {
        return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f');
    }

    private void failParse(final ZcodeCommandInStream in, final ZcodeResponseStatus errorStatus, final String message) {
        status = errorStatus;
        errorMessage = message;
        in.close();
        end = Zchars.EOL_SYMBOL.ch;
    }

    public boolean parseSingleCommand(final ZcodeCommandInStream in, final ZcodeCommandSequence sequence) {
        ZcodeBigField target = big;
        in.open();
        reset();
        in.eatWhitespace();
        if (!in.hasNext()) {
            failParse(in, ZcodeResponseStatus.PARSE_ERROR, "No command present");
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
                        failParse(in, ZcodeResponseStatus.PARSE_ERROR, "Same field appears twice");
                        return false;
                    } else if (!parseHexField(in, c)) {
                        failParse(in, ZcodeResponseStatus.TOO_BIG, "Too many fields");
                        return false;
                    }
                } else if (c == Zchars.BIGFIELD_PREFIX_MARKER.ch) {
                    if (target.getLength() != 0) {
                        failParse(in, ZcodeResponseStatus.PARSE_ERROR, "Multiple big fields");
                        return false;
                    }
                    target.setIsString(false);
                    byte d = getHex(in.peek());

                    while (in.hasNext() && isHex(in.peek())) {
                        d = getHex(in.read());
                        d <<= 4;
                        if (!in.hasNext() || !isHex(in.peek())) {
                            failParse(in, ZcodeResponseStatus.PARSE_ERROR, "Big field odd digits");
                            return false;
                        }
                        d += getHex(in.read());
                        if (!target.addByteToBigField(d)) {
                            if (target == big) {
                                isHuge = true;
                                target = huge;
                                big.copyTo(huge);
                                huge.addByteToBigField(d);
                            } else {
                                failParse(in, ZcodeResponseStatus.TOO_BIG, "Big field too long");
                                return false;
                            }
                        }
                    }
                } else if (c == Zchars.BIGFIELD_QUOTE_MARKER.ch) {
                    if (target.getLength() != 0) {
                        failParse(in, ZcodeResponseStatus.PARSE_ERROR, "Multiple big fields");
                        return false;
                    }
                    target.setIsString(true);
                    c = 0;
                    while (in.hasNext()) {
                        c = in.read();
                        if (c != Zchars.BIGFIELD_QUOTE_MARKER.ch) {
                            if (c == '\\') {
                                if (!in.hasNext()) {
                                    break;
                                }
                                c = in.read();
                                if (c == 'n') {
                                    c = Zchars.EOL_SYMBOL.ch;
                                }
                            }
                            if (!target.addByteToBigField((byte) c)) {
                                if (target == big) {
                                    isHuge = true;
                                    target = huge;
                                    target.setIsString(true);
                                    big.copyTo(huge);
                                    huge.addByteToBigField((byte) c);
                                } else {
                                    failParse(in, ZcodeResponseStatus.TOO_BIG, "Big field too long");
                                    return false;
                                }
                            }
                        } else {
                            break;
                        }
                    }
                    if (c != Zchars.BIGFIELD_QUOTE_MARKER.ch) {
                        failParse(in, ZcodeResponseStatus.PARSE_ERROR, "Command sequence ended before end of big string field");
                        return false;
                    }
                } else {
                    failParse(in, ZcodeResponseStatus.PARSE_ERROR, "Unknown field marker with character code " + (int) c);
                    return false;
                }
            } else {
                in.close();
                end = in.read();
                if (getCommand() == null) {
                    failParse(in, ZcodeResponseStatus.UNKNOWN_CMD, "Command not known");
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
        return end == Zchars.EOL_SYMBOL.ch;
    }

    public ZcodeResponseStatus getStatus() {
        return status;
    }

    public String getErrorMessage() {
        return errorMessage;
    }

    public void fail(final String errorMessage, final ZcodeResponseStatus status) {
        this.errorMessage = errorMessage;
        this.status = status;
    }
}
