package org.zcode.zcode_acceptance_tests.acceptancetest_asserts;

import java.io.IOException;
import java.io.PushbackInputStream;

public class ZcodeAcceptanceTestResponse {
    private final ZcodeAcceptanceTestFieldMap map = new ZcodeAcceptanceTestFieldMap();
    private final ZcodeAcceptanceTestBigField big = new ZcodeAcceptanceTestBigField();
    private ZcodeAcceptanceTestResponseStatus status = ZcodeAcceptanceTestResponseStatus.OK;
    private char end;

    public ZcodeAcceptanceTestFieldMap getFields() {
        return map;
    }

    public ZcodeAcceptanceTestBigField getBigField() {
        return big;
    }

    public ZcodeAcceptanceTestResponseStatus getStatus() {
        return status;
    }

    public char getEnd() {
        return end;
    }

    private byte getHex(int c) {
        return (byte) (c >= 'a' ? c - 'a' + 10 : c - '0');
    }

    private boolean isHex(int c) {
        return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f');
    }

    private String parseHexField(PushbackInputStream in, char f, String current) throws IOException {
        String hex = "";
        int c = in.read();
        while (c != -1 && isHex(c)) {
            hex += (char) c;
            c = in.read();
        }
        if (c != -1) {
            in.unread(c);
        }
        current += hex;
        if (hex.length() == 0) {
            map.addField(f, (byte) 0);
        } else {
            if (hex.length() % 2 == 1) {
                map.addField(f, getHex(hex.charAt(0)));
            }
            for (int i = hex.length() % 2; i < hex.length(); i += 2) {
                map.addField(f, (byte) ((getHex(hex.charAt(i)) << 4) | getHex(hex.charAt(i + 1))));
            }
        }
        return current;
    }

    public String parseCommand(PushbackInputStream in, ZcodeAcceptanceTestResponseSequence sequence, String current) {
        try {
            int c = in.read();
            while (c == ' ' || c == '\t' || c == '\r') {
                c = in.read();
            }
            if (c == '#') {
                if (sequence.isEmpty()) {
                    status = ZcodeAcceptanceTestResponseStatus.SKIP_COMMAND;
                    while (c != -1 && c != '\n') {
                        c = in.read();
                    }
                    end = '\n';
                    return current;
                } else {
                    current += (char) c;
                    throw new AssertionError("Response begins with #, but not start of Response Sequence\nCurrent sequence: " + current);
                }
            }
            if (c == '*') {
                if (sequence.isEmpty()) {
                    sequence.setBroadcast();
                    current += (char) c;
                    c = in.read();
                } else {
                    current += (char) c;
                    throw new AssertionError("Response begins with *, but not start of Response Sequence\nCurrent sequence: " + current);
                }
            }
            if (c == '!') {
                if (sequence.isBroadcast()) {
                    current += (char) c;
                    throw new AssertionError("Response cannot be both broadcast and notification\nCurrent sequence: " + current);
                } else if (sequence.isEmpty()) {
                    sequence.setNotification();
                    current += (char) c;
                    c = in.read();
                } else {
                    current += (char) c;
                    throw new AssertionError("Response begins with !, but not start of Response Sequence\nCurrent sequence: " + current);
                }
            }
            while (true) {
                if (c != -1) {
                    while (c == ' ' || c == '\t' || c == '\r') {
                        c = in.read();
                    }
                }
                if (c != -1 && c != '&' && c != '|' && c != '\n') {
                    if (c >= 'A' && c <= 'Z') {
                        current += (char) c;
                        current = parseHexField(in, (char) c, current);
                    } else if (c == '+') {
                        current += (char) c;
                        if (big.getField().size() != 0) {
                            throw new AssertionError("Response contains multiple big fields\nCurrent sequence: " + current);
                        }
                        big.setIsString(false);
                        byte d;
                        c = in.read();
                        while (c != -1 && isHex(c)) {
                            d = (byte) (getHex(c) << 4);
                            current += (char) c;
                            c = in.read();
                            current += (char) c;
                            if (c == -1 || !isHex(c)) {
                                throw new AssertionError("Response contains big data field with odd digit number\nCurrent sequence: " + current);
                            }
                            d += getHex(c);
                            big.addByte(d);
                            c = in.read();
                        }
                        if (c != -1) {
                            in.unread(c);
                        }
                    } else if (c == '"') {
                        current += (char) c;
                        if (big.getField().size() != 0) {
                            throw new AssertionError("Response contains multiple big fields\nCurrent sequence: " + current);
                        }
                        big.setIsString(true);
                        c = in.read();
                        while (c != -1) {
                            if (c != '"') {
                                if (c == '\\') {
                                    c = in.read();
                                    if (c == -1) {
                                        break;
                                    }
                                    if (c == 'n') {
                                        c = '\n';
                                    }
                                }
                                big.addByte((byte) c);
                            } else {
                                break;
                            }
                            c = in.read();
                        }
                        if (c != '"') {
                            throw new AssertionError("Response ended before end of big string field\nCurrent sequence: " + current);
                        }
                        current += (char) c;
                    } else {
                        current += (char) c;
                        throw new AssertionError("Unknown field marker: " + c + "\nCurrent sequence: " + current);
                    }
                } else {
                    if (c == -1) {
                        end = '\n';
                    } else {
                        current += (char) c;
                        end = (char) c;
                    }
                    return current;
                }
                c = in.read();
            }
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

}
