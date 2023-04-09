package org.zcode.javareceiver.tokenizer;

import java.util.Arrays;

public class ZcodeTokenDataIterator {
    private ZcodeTokenRingBuffer buffer = null;

    private byte  key                    = 0;
    private int   position               = 0;
    private short remainingSegmentLength = 0; // uint8

    public void set(ZcodeTokenRingBuffer buffer, byte key, int position) {
        this.buffer = buffer;
        this.key = key;
        this.position = position;
        this.remainingSegmentLength = buffer.data[position];
    }

    public byte getKey() {
        return key;
    }

    public byte next() {
        if (remainingSegmentLength > 0) {
            byte tmp = buffer.data[position];
            position = buffer.offset(position, 1);
            return tmp;
        } else if (buffer.readLimit == position || buffer.data[buffer.offset(position, 1)] != ZcodeTokenRingBuffer.TOKEN_EXTENSION) {
            throw new IllegalStateException("Cannot call next to go beyond current Token");
        } else {
            remainingSegmentLength = buffer.data[position];
            position = buffer.offset(position, 2);
            byte tmp = buffer.data[position];
            position = buffer.offset(position, 1);
            return tmp;
        }
    }

    public boolean hasNext() {
        return remainingSegmentLength > 0 || (position != buffer.readLimit && buffer.data[buffer.offset(position, 1)] == ZcodeTokenRingBuffer.TOKEN_EXTENSION);
    }

    public int calculateRemainingLength() {
        int len    = remainingSegmentLength;
        int tmpPos = buffer.offset(position, remainingSegmentLength);
        while (tmpPos != buffer.readLimit && buffer.data[buffer.offset(tmpPos, 1)] == ZcodeTokenRingBuffer.TOKEN_EXTENSION) {
            len += buffer.data[tmpPos];
            tmpPos = buffer.offset(position, buffer.data[tmpPos] + 2);
        }
        return len;
    }

    public byte[] getContiguous() {
        return Arrays.copyOfRange(buffer.data, position, Math.max(position + remainingSegmentLength, buffer.data.length));
    }
}
