package org.zcode.javaclient.parsing;

import java.io.ByteArrayOutputStream;

public class Comment {
    private final byte[] commentBytes;

    Comment(CommentBuilder commentBuilder) {
        commentBytes = commentBuilder.baos.toByteArray();
    }

    public byte[] toBytes() {
        return commentBytes.clone();
    }

    public static CommentBuilder builder() {
        return new CommentBuilder();
    }

    public static class CommentBuilder {
        private final ByteArrayOutputStream baos;

        private CommentBuilder() {
            baos = new ByteArrayOutputStream();
        }

        public void add8(byte b) {
            baos.write(b);
        }

        public Comment build() {
            return new Comment(this);
        }
    }
}
