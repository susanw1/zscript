package org.zcode.javareceiver.core;

public class ZcodeStatus {
    public static final byte SUCCESS              = 0;
    public static final byte INTERNAL_ERROR       = 1; // impossible condition hit
    public static final byte BUFFER_OVR_ERROR     = 2; // buffer overflow
    public static final byte SYNTAX_ERROR         = 3; // e.g. fields too long, odd length big field, missing end quote
    public static final byte SEQUENCE_ERROR       = 4; // giving multiple lock fields, giving an echo on a comment, etc.
    public static final byte TOKEN_FORMAT_ERROR   = 5; // giving too long tokens, or tokens which are invalid in that position
    public static final byte ADDRESSING_ERROR     = 6; // addressing non-existent things, or addressing operations which can't be performed
    public static final byte COMMAND_FORMAT_ERROR = 7; // giving badly formed commands, including commands which don't exist or commands with invalid fields

    public static final byte COMMAND_ERROR = 0xf;  // fatal command error (e.g. I2C bus jam)
    public static final byte COMMAND_FAIL  = 0x10; // non-fatal command failure (e.g. I2C nack)
}
