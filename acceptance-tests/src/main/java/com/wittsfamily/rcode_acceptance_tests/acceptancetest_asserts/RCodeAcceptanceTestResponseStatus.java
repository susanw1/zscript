package com.wittsfamily.rcode_acceptance_tests.acceptancetest_asserts;

public enum RCodeAcceptanceTestResponseStatus {
    OK(0),
    RESP_TOO_LONG(1),
    UNKNOWN_ERROR(2),
    PARSE_ERROR(3),
    UNKNOWN_CMD(4),
    MISSING_PARAM(5),
    BAD_PARAM(6),
    NOT_ACTIVATED(7),
    TOO_BIG(8),
    CMD_FAIL(0x10),
    NOT_EXECUTED(0x11),
    SKIP_COMMAND(0xFF);

    private final int value;

    private RCodeAcceptanceTestResponseStatus(int value) {
        this.value = value;
    }

    public int getValue() {
        return value;
    }
}
