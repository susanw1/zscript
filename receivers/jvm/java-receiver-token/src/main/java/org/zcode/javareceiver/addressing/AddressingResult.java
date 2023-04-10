package org.zcode.javareceiver.addressing;

public class AddressingResult {
    public static final byte ADDRESSING_NOT_STARTED = 0;
    public static final byte ADDRESSING_STARTED      = 1;
    public static final byte ADDRESSING_SUCCESS      = 2;
    public static final byte ADDRESSING_FAILED       = 3;

    public byte addressingResponse = ADDRESSING_NOT_STARTED;
}
