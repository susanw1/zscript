package net.zscript.javareceiver.core;

public class ZcodeStatus {
    /** command success */
    public static final byte SUCCESS = 0x0;

    /** command failure */
    public static final byte COMMAND_FAIL                    = 0x1;
    /** command failed in order to control the program flow */
    public static final byte COMMAND_FAIL_CONTROL            = 0x2;
    /** target of command either non-existent or not responding */
    public static final byte COMMAND_NO_TARGET               = 0x3;
    /** target of command does not support the requested functionality */
    public static final byte COMMAND_TARGET_DOES_NOT_SUPPORT = 0x4;
    /** data transmission not successful - may have failed or only been partial */
    public static final byte COMMAND_DATA_NOT_TRANSMITTED    = 0x8;
    /** data reception not successful - may have failed or only been partial */
    public static final byte COMMAND_DATA_NOT_RECEIVED       = 0x9;

    /** buffer overflow */
    public static final byte BUFFER_OVR_ERROR      = 0x10;
    /** impossible condition hit */
    public static final byte INTERNAL_ERROR        = 0x11;
    /** channel not activated for non-core command */
    public static final byte NOT_ACTIVATED         = 0x12;
    /** command error in order to control the program flow */
    public static final byte COMMAND_ERROR_CONTROL = 0x13;
    /** Response too long */
    public static final byte RESPONSE_TOO_LONG     = 0x14;

    /** numeric field too long */
    public static final byte FIELD_TOO_LONG      = 0x20;
    /** numeric field too long */
    public static final byte ODD_LENGTH          = 0x21;
    /** missing end quote */
    public static final byte UNTERMINATED_STRING = 0x22;
    /** string escaping invalid */
    public static final byte ESCAPING_ERROR      = 0x23;
    /** illegal character given as token key */
    public static final byte ILLEGAL_KEY         = 0x24;

    /** locks field too long or repeated */
    public static final byte INVALID_LOCKS   = 0x30;
    /** echo field too long or repeated */
    public static final byte INVALID_ECHO    = 0x31;
    /** comment with non-comment fields found */
    public static final byte INVALID_COMMENT = 0x32;

    /** two fields (in the same command) with the same key found */
    public static final byte REPEATED_KEY      = 0x43;
    /** field not valid in this position */
    public static final byte INVALID_KEY       = 0x44;
    /** given command does not exist or not specified */
    public static final byte COMMAND_NOT_FOUND = 0x45;
    /** given address does not exist or cannot be addressed */
    public static final byte ADDRESS_NOT_FOUND = 0x46;

    /** a required field was not given */
    public static final byte MISSING_KEY        = 0x50;
    /** a field or big field was given with a value which is not supported */
    public static final byte VALUE_OUT_OF_RANGE = 0x51;
    /** data transmission/reception exceeds maximum transmission length */
    public static final byte DATA_TOO_LONG      = 0x52;

    /** generic failure mid-execution */
    public static final byte EXECUTION_ERROR = (byte) 0x80;

    /** a peripheral required for the command has failed fatally */
    public static final byte PERIPHERAL_ERROR        = (byte) 0x90;
    /** a peripheral required for the command is in a state it cannot exit (and may require rebooting) */
    public static final byte PERIPHERAL_JAM          = (byte) 0x91;
    /** a peripheral required for the command is not currently enabled (and may require other commands or activity to enable) */
    public static final byte PERIPHERAL_DISABLED     = (byte) 0x92;
    /** a peripheral required for the command is not attached (and may require physical intervention to attach) */
    public static final byte PERIPHERAL_DISCONNECTED = (byte) 0x93;
    /** a peripheral required for the command has sustained permanent damage (and may require replacement) */
    public static final byte PERIPHERAL_BROKEN       = (byte) 0x94;

    public static boolean isSuccess(final byte code) {
        return code == SUCCESS;
    }

    public static boolean isError(final byte code) {
        return (code & ~0xf) != 0;
    }

    public static boolean isFailure(final byte code) {
        return (code & ~0xf) == 0 && code != SUCCESS;
    }

}
