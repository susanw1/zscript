use core::ops::Index;
use core::ops::IndexMut;

struct TokenBuffer {
    data: [u8; 256],
    write_start: u16,
    read_start: u16,
}
const TOKEN_EXTENSION:u8 = 0x80;

impl TokenBuffer {
    pub fn offset(index: u16, offset: u16) -> u16 {
        index + offset
    }
    pub fn new() -> Self {
        Self { data: [0; 256], write_start: 0, read_start: 0 }
    }
    pub fn is_marker(key: u8) -> bool {
        (key & 0xc0) != 0
    }
}

impl Index<u16> for TokenBuffer {
    type Output = u8;
    fn index(&self, index: u16) -> &u8 {
        &self.data[index as usize]
    }
}

impl IndexMut<u16> for TokenBuffer {
    fn index_mut(&mut self, index: u16) -> &mut u8 {
        &mut self.data[index as usize]
    }
}

pub struct TokenWriter<'a> {
    buffer: &'a mut TokenBuffer,
    write_last_len: u16,
    write_cursor: u16,
    in_nibble: bool,
    numeric: bool,
}

impl<'a> TokenWriter<'a> {
    pub fn start_token(&mut self, key: u8, numeric: bool) {
        self.end_token();
        self.numeric = numeric;
        self.write_new_token_start(key);
    }

    pub fn end_token(&mut self) {
        if self.in_nibble {
            if self.numeric {
                // if odd nibble count, then shuffle nibbles through token's data to ensure "right-aligned", eg 4ad0 really means 04ad
                let mut hold: u8 = 0;
                let mut pos = TokenBuffer::offset(self.buffer.write_start, 1);
                loop {
                    pos = TokenBuffer::offset(pos, 1);
                    let tmp: u8 = self.buffer[pos] & 0xF;
                    self.buffer[pos] = hold | (self.buffer[pos] >> 4) & 0xF;
                    hold = tmp << 4;
                    if pos == self.write_cursor {
                        break;
                    }
                }
            }
            self.move_cursor();
        }

        self.buffer.write_start = self.write_cursor;
        self.in_nibble = false;
    }

    pub fn continue_token_byte(&mut self, b: u8) {
        if self.in_nibble {
            panic!("Cannot write a byte while midway through a nibble");
        }
        if self.is_token_complete() {
            panic!("Cannot write a byte without key");
        }

        if self.buffer[self.write_last_len] == 0xff {
            if self.numeric {
                panic!("Cannot write a numeric field longer than 255 bytes");
            }
            self.write_new_token_start(TOKEN_EXTENSION);
        }
        self.buffer[self.write_cursor] = b;
        self.move_cursor();
        self.buffer[self.write_last_len] += 1;
    }

    pub fn continue_token_nibble(&mut self, nibble: u8) {
        if self.is_token_complete() {
            panic!("Cannot write a nibble without key");
        }
        if nibble > 0xf {
            panic!("Nibble value out of range");
        }

        if self.in_nibble {
            self.buffer[self.write_cursor] |= nibble;
            self.move_cursor();
        } else {
            if self.buffer[self.write_last_len] == 0xff {
                if self.numeric {
                    panic!("Cannot write a numeric field longer than 255 bytes");
                }
                self.write_new_token_start(TOKEN_EXTENSION);
            }
            self.buffer[self.write_cursor] = nibble << 4;
            self.buffer[self.write_last_len] += 1;
        }
        self.in_nibble = !self.in_nibble;
    }

    pub fn is_token_complete(&self) -> bool {
        return self.buffer.write_start == self.write_cursor;
    }
    fn write_new_token_start(&mut self, key: u8) {
        if TokenBuffer::is_marker(key) {
            panic!("invalid token [key=0x{}]", key);
        }
        self.buffer[self.write_cursor] = key;
        self.move_cursor();
        self.buffer[self.write_cursor] = 0;
        self.write_last_len = self.write_cursor;
        self.move_cursor();
    }

    pub fn write_marker(&mut self, key: u8) {
        if !TokenBuffer::is_marker(key) {
            panic!("invalid marker [key=0x{}]", key);
        }
        self.end_token();
        self.buffer[self.write_cursor] = key;
        self.move_cursor();
        self.end_token();
    }

    pub fn fail(&mut self, error_code: u8) {
        if !self.is_token_complete() {
            // reset current token back to writeStart
            self.write_cursor = self.buffer.write_start;
            self.in_nibble = false;
        }
        self.write_marker(error_code);
    }
    fn move_cursor(&mut self) {
        let next_cursor = TokenBuffer::offset(self.write_cursor, 1);
        if next_cursor == self.buffer.read_start {
            // this should never happen - someone should have made sure there was space
            panic!("Out of buffer - should have reserved more");
        }
        self.write_cursor = next_cursor;
    }

    pub fn get_current_write_token_key(&self) -> u8 {
        if self.is_token_complete() {
            panic!("no token being written");
        }
        return self.buffer[self.buffer.write_start];
    }

    pub fn get_current_write_token_length(&self) -> u8 {
        if self.is_token_complete() {
            panic!("no token being written");
        }
        return self.buffer[TokenBuffer::offset(self.buffer.write_start, 1)];
    }

    pub fn is_in_nibble(&self) -> bool { self.in_nibble }

    pub fn check_available_capacity(&self, size: u16) -> bool {
        let available: u16 = if self.write_cursor >= self.buffer.read_start { self.buffer.data.len() as u16 } else { 0 } + self.buffer.read_start - self.write_cursor - 1;
        return size <= available;
    }
}
