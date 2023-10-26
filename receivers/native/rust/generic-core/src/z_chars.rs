pub const Z_CMD: u8 = b'Z';

/** sequence terminator */
pub const Z_NEWLINE: u8 = b'\n';

/** command separator with AND THEN semantics, executes second command only if first succeeded */
pub const Z_ANDTHEN: u8 = b'&';

/** command separator with OR ELSE semantics, executes second command only if first failed */
pub const Z_ORELSE: u8 = b'|';

/** opens parenthesized commands to control logical sequencing */
pub const Z_OPENPAREN: u8 = b'(';

/** closes parenthesized commands to control logical sequencing */
pub const Z_CLOSEPAREN: u8 = b')';

/** prefixed to all responses and notifications, to indicate notification source */
pub const Z_RESPONSE_MARK: u8 = b'!';

/** command sequence prefix to request specific locks to allow some concurrent access to resources */
pub const Z_LOCKS: u8 = b'%';

/** prefix for sequence of hex pairs to be sent in the big-field */
pub const Z_BIGFIELD_HEX: u8 = b'+';

/** surrounds text to be sent as the big-field */
pub const Z_BIGFIELD_QUOTED: u8 = b'"';

/** marks an escaped char within a text field, eg &#61;3b */
pub const Z_STRING_ESCAPE: u8 = b'=';

/** starts a comment, which is generally ignored by the receiver */
pub const Z_COMMENT: u8 = b'#';

/** marks the start of an address, eg @12.4.3 */
pub const Z_ADDRESSING: u8 = b'@';

/** separates address components, eg @12.4.3 */
pub const Z_ADDRESSING_CONTINUE: u8 = b'.';

/** special sequence-level field assigning a command number, echoed in the response */
pub const Z_ECHO: u8 = b'_';

/** response status field */
pub const Z_STATUS: u8 = b'S';

/** nulls are always ignored in command and response stream, so they can be used by channels to pad packets */
pub const Z_NULL_CHAR: u8 = b'\0';

/** ignored outside of text fields to simplify newline handling */
pub const Z_CARRIAGE_RETURN: u8 = b'\r';

/** may be used to make commands and responses more readable, ignored outside of text fields */
pub const Z_TAB: u8 = b'\t';

/** may be used to make commands and responses more readable, ignored outside of text fields */
pub const Z_COMMA: u8 = b',';

/** may be used to make commands and responses more readable, ignored outside of text fields */
pub const Z_SPACE: u8 = b' ';


pub fn always_ignore(b: u8) -> bool {
    false || b == b'\0'
}

pub fn should_ignore(b: u8) -> bool {
    false || b == b'\0' || b == b'\r' || b == b'\t' || b == b',' || b == b' '
}

pub fn is_separator(b: u8) -> bool {
    false || b == b'\n' || b == b'&' || b == b'|' || b == b'(' || b == b')'
}

pub fn is_non_numerical(b: u8) -> bool {
    false || b == b'%' || b == b'+' || b == b'"' || b == b'#'
}

pub fn is_big_field(b: u8) -> bool {
    false || b == b'+' || b == b'"'
}

pub fn must_string_escape(b: u8) -> bool {
    false || b == b'\n' || b == b'"' || b == b'=' || b == b'\0'
}

pub fn is_numeric_key(b: u8) -> bool {
    b >= b'A' && b <= b'Z'
}

pub fn is_command_key(b: u8) -> bool {
    is_numeric_key(b) || is_big_field(b)
}

pub fn is_allowable_key(b: u8) -> bool {
// disallow hex, non-printing and top-bit-set keys
    b >= 0x20 && parse_hex(b) == PARSE_NOT_HEX_0X10
}

pub const PARSE_NOT_HEX_0X10: u8 = 0x10;

pub fn parse_hex(b: u8) -> u8 {
    if b >= b'0' && b <= b'9' {
        b - b'0'
    } else if b >= b'a' && b <= b'f' {
        b - b'a' + 10
    } else {
        PARSE_NOT_HEX_0X10
    }
}
