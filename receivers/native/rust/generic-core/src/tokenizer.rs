use crate::token_buffer::TokenWriter;
use crate::z_chars;

pub static NORMAL_SEQUENCE_END: u8 = 0xf0;
pub static ERROR_BUFFER_OVERRUN: u8 = 0xf1;
pub static ERROR_CODE_ODD_BIGFIELD_LENGTH: u8 = 0xf2;
pub static ERROR_CODE_FIELD_TOO_LONG: u8 = 0xf3;
pub static ERROR_CODE_STRING_NOT_TERMINATED: u8 = 0xf4;
pub static ERROR_CODE_STRING_ESCAPING: u8 = 0xf5;
pub static ERROR_CODE_ILLEGAL_TOKEN: u8 = 0xf6;

pub static CMD_END_ANDTHEN: u8 = 0xe1;
pub static CMD_END_ORELSE: u8 = 0xe2;
pub static CMD_END_OPEN_PAREN: u8 = 0xe3;
pub static CMD_END_CLOSE_PAREN: u8 = 0xe4;

pub static ADDRESSING_FIELD_KEY: u8 = 0x80;

struct Tokenizer<'a> {
    writer: &'a mut TokenWriter<'a>,
    skip_to_nl: bool,
    buffer_ovr: bool,
    tokenizer_error: bool,
    numeric: bool,
    addressing: bool,

    is_text: bool,
    is_normal_string: bool,
    escaping_count: u8,
}

impl<'a> Tokenizer<'a> {
    pub fn new(writer: &'a mut TokenWriter<'a>) -> Tokenizer<'a> {
        Tokenizer { writer, skip_to_nl: false, buffer_ovr: false, tokenizer_error: false, numeric: false, addressing: false, is_text: false, is_normal_string: false, escaping_count: 0 }
    }

    fn reset_flags(&mut self) {
        self.skip_to_nl = false;
        self.buffer_ovr = false;
        self.tokenizer_error = false;
        self.numeric = false;
        self.addressing = false;
        self.is_text = false;
        self.is_normal_string = false;
        self.escaping_count = 0;
    }

    /**
     * If a channel becomes aware that it has lost (or is about to lose) data, either because the channel has run out of buffer, or because the TokenBuffer is out of room, then it
     * can signal the Tokenizer to mark the current command sequence as overrun.
     */
    fn data_lost(&mut self) {
        if !self.buffer_ovr {
            self.writer.fail(ERROR_BUFFER_OVERRUN);
            self.buffer_ovr = true;
        }
    }

    /**
     * Determine whether there is guaranteed capacity for another byte of input, in the worst case without knowing what that byte is.
     *
     * @return true if capacity definitely exists, false otherwise.
     */
    fn check_capacity(&self) -> bool {
        // worst case is... TODO: review this!
        return self.writer.check_available_capacity(3);
    }

    /**
     * TODO: Review: this seems a bit simple...
     *
     * @param b
     * @return
     */
    fn offer(&mut self, b: u8) -> bool {
        if self.check_capacity()// || self.writer.get_flags().is_reader_blocked()
        {
            self.accept(b);
            return true;
        }
        return false;
    }
    // TODO: Discuss dataLost changes.

    /**
     * Process a byte of Zscript input into the parser.
     *
     * @param b the new byte of zscript input
     */
    fn accept(&mut self, b: u8) {
        if !self.is_text && z_chars::should_ignore(b) || z_chars::always_ignore(b) {
            return;
        }

        if self.buffer_ovr || self.tokenizer_error || self.skip_to_nl {
            if b == z_chars::Z_NEWLINE {
                if self.skip_to_nl {
                    if !self.check_capacity() {
                        self.data_lost();
                        return;
                    }
                    self.writer.write_marker(NORMAL_SEQUENCE_END);
                }
                self.reset_flags();
            }
            return;
        }
        if !self.check_capacity() {
            self.data_lost();
            return;
        }


        if self.writer.is_token_complete() {
            self.start_new_token(b);
            return;
        }

        if self.is_text {
            // "text" is broadly interpreted: we're pushing non-numeric bytes into a current token
            self.accept_text(b);
            return;
        }

        let hex: u8 = z_chars::parse_hex(b);
        if hex != z_chars::PARSE_NOT_HEX_0X10 {
            if self.numeric {
                // Check field length
                let current_length: u8 = self.writer.get_current_write_token_length();
                if current_length == 2 && !self.writer.is_in_nibble() {
                    self.writer.fail(ERROR_CODE_FIELD_TOO_LONG);
                    self.tokenizer_error = true;
                    return;
                }
                // Skip leading zeros
                if current_length == 0 && hex == 0 {
                    return;
                }
            }
            self.writer.continue_token_nibble(hex);
            return;
        }

        // Check big field odd length
        if !self.numeric && self.writer.get_current_write_token_key() == z_chars::Z_BIGFIELD_HEX && self.writer.is_in_nibble() {
            self.writer.fail(ERROR_CODE_ODD_BIGFIELD_LENGTH);
            self.tokenizer_error = true;
            if b == z_chars::Z_NEWLINE {
                // interesting case: the error above could be caused by b==Z_NEWLINE, but we've written an error marker, so just reset and return
                self.reset_flags();
            }
            return;
        }

        self.start_new_token(b);
    }

    fn accept_text(&mut self, b: u8) {
        if b == z_chars::Z_NEWLINE {
            if self.is_normal_string {
                self.writer.fail(ERROR_CODE_STRING_NOT_TERMINATED);
            } else {
                self.writer.write_marker(NORMAL_SEQUENCE_END);
            }
            // we've written some marker, so reset as per the newline:
            self.reset_flags();
        } else if self.escaping_count > 0 {
            let hex: u8 = z_chars::parse_hex(b);
            if hex == z_chars::PARSE_NOT_HEX_0X10 {
                self.writer.fail(ERROR_CODE_STRING_ESCAPING);
                self.tokenizer_error = true;
            } else {
                self.writer.continue_token_nibble(hex);
                self.escaping_count -= 1;
            }
        } else if self.is_normal_string && b == z_chars::Z_BIGFIELD_QUOTED {
            self.writer.end_token();
            self.is_text = false;
        } else if self.is_normal_string && b == z_chars::Z_STRING_ESCAPE {
            self.escaping_count = 2;
        } else {
            self.writer.continue_token_byte(b);
        }
    }

    fn start_new_token(&mut self, b: u8) {
        if b == z_chars::Z_NEWLINE {
            self.writer.write_marker(NORMAL_SEQUENCE_END);
            self.reset_flags();
            return;
        }

        if self.addressing && b != z_chars::Z_ADDRESSING_CONTINUE {
            self.writer.start_token(ADDRESSING_FIELD_KEY, false);
            self.writer.continue_token_byte(b);
            self.addressing = false;
            self.is_text = true;
            self.escaping_count = 0;
            self.is_normal_string = false;
            return;
        }

        if z_chars::is_separator(b) {
            let marker: u8 = match b {
                z_chars::Z_ANDTHEN => CMD_END_ANDTHEN,
                z_chars::Z_ORELSE => CMD_END_ORELSE,
                z_chars::Z_OPENPAREN => CMD_END_OPEN_PAREN,
                z_chars::Z_CLOSEPAREN => CMD_END_CLOSE_PAREN,
                _ => panic!("Unknown separator")
            };
            self.writer.write_marker(marker);
            return;
        }

        if b == z_chars::Z_ADDRESSING {
            self.addressing = true;
        }

        if !z_chars::is_allowable_key(b) {
            self.writer.fail(ERROR_CODE_ILLEGAL_TOKEN);
            self.tokenizer_error = true;
            return;
        }

        self.numeric = !z_chars::is_non_numerical(b);
        self.is_text = false;
        if b != z_chars::Z_COMMENT {
            self.writer.start_token(b, self.numeric);
        } else {
            self.skip_to_nl = true;
            return;
        }

        if b == z_chars::Z_BIGFIELD_QUOTED {
            self.is_text = true;
            self.is_normal_string = true;
            self.escaping_count = 0;
        }
    }
}