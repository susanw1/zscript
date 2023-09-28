package net.zscript.javaclient.core;

import java.io.IOException;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.ListIterator;
import java.util.Map;
import java.util.Optional;

import net.zscript.javareceiver.tokenizer.TokenBuffer;
import net.zscript.javareceiver.tokenizer.TokenBufferIterator;
import net.zscript.javareceiver.tokenizer.Tokenizer;
import net.zscript.model.ZscriptModel;
import net.zscript.model.components.Zchars;

public class ResponseExecutionPath {
    static class Response {
        private final Response next;

        private final byte separator;

        private final Map<Byte, Integer> fields;
        private final List<byte[]>       bigFields;

        private final boolean hasClash;

        Response(Response next, byte separator, Map<Byte, Integer> fields, List<byte[]> bigFields, boolean hasClash) {
            this.next = next;
            this.separator = separator;
            this.fields = fields;
            this.bigFields = bigFields;
            this.hasClash = hasClash;
        }

    }

    static class ResponseBuilder {
        TokenBuffer.TokenReader.ReadToken start = null;

        ResponseBuilder next      = null;
        byte            seperator = 0;

        public void setNext(ResponseBuilder next) {
            this.next = next;
        }

        public TokenBuffer.TokenReader.ReadToken getStart() {
            return start;
        }

        public void setStart(TokenBuffer.TokenReader.ReadToken token) {
            this.start = token;
        }

        public Response generateResponse(Map<ResponseBuilder, Response> otherResponses) {
            Map<Byte, Integer> fields    = new HashMap<>();
            List<byte[]>       bigFields = new ArrayList<>();

            boolean hasClash = false;

            TokenBufferIterator iterator = start.getNextTokens();
            for (Optional<TokenBuffer.TokenReader.ReadToken> opt = iterator.next(); opt.filter(t -> !t.isMarker()).isPresent(); opt = iterator.next()) {
                TokenBuffer.TokenReader.ReadToken token = opt.get();
                if (Zchars.isBigField(token.getKey())) {
                    byte[] data = new byte[token.getDataSize()];

                    int i = 0;
                    for (Iterator<Byte> iter = token.blockIterator(); iter.hasNext(); ) {
                        data[i++] = iter.next();
                    }
                    bigFields.add(data);
                } else {
                    if (fields.containsKey(token.getKey()) || token.getDataSize() > 2) {
                        hasClash = true;
                    } else {
                        fields.put(token.getKey(), token.getData16());
                    }
                }
            }
            return new Response(otherResponses.get(next), seperator, Collections.unmodifiableMap(fields), Collections.unmodifiableList(bigFields), hasClash);
        }
    }

    private static List<ResponseBuilder> createLinkedPath(TokenBuffer.TokenReader.ReadToken start) {
        List<ResponseBuilder> builders = new ArrayList<>();

        ResponseBuilder last = new ResponseBuilder();
        builders.add(last);
        TokenBufferIterator iterator = start.getNextTokens();
        for (Optional<TokenBuffer.TokenReader.ReadToken> opt = iterator.next(); opt.isPresent(); opt = iterator.next()) {
            TokenBuffer.TokenReader.ReadToken token = opt.get();
            if (last.getStart() == null) {
                last.setStart(token);
            }
            if (token.isMarker()) {
                ResponseBuilder next = new ResponseBuilder();
                builders.add(next);
                if (token.getKey() == Tokenizer.CMD_END_ANDTHEN) {
                    last.seperator = Zchars.Z_ANDTHEN;
                } else if (token.getKey() == Tokenizer.CMD_END_ORELSE) {
                    last.seperator = Zchars.Z_ORELSE;
                } else if (token.getKey() == Tokenizer.CMD_END_OPEN_PAREN) {
                    last.seperator = Zchars.Z_OPENPAREN;
                } else if (token.getKey() == Tokenizer.CMD_END_CLOSE_PAREN) {
                    last.seperator = Zchars.Z_CLOSEPAREN;
                } else if (token.isSequenceEndMarker()) {
                    break;
                } else {
                    throw new IllegalStateException("Unknown separator: " + Integer.toHexString(Byte.toUnsignedInt(token.getKey())));
                }
                last.next = next;
                last = next;
            }
        }
        return builders;
    }

    public static ResponseExecutionPath parse(TokenBuffer.TokenReader.ReadToken start) {
        List<ResponseBuilder> builders = createLinkedPath(start);

        Map<ResponseBuilder, Response> commands = new HashMap<>();
        for (ListIterator<ResponseBuilder> iter = builders.listIterator(builders.size()); iter.hasPrevious(); ) {
            ResponseBuilder b = iter.previous();
            if (b.start != null) {
                commands.put(b, b.generateResponse(commands));
            }
        }
        return new ResponseExecutionPath(commands.get(builders.get(0)));
    }

    private final Response firstResponse;

    private ResponseExecutionPath(Response firstResponse) {
        this.firstResponse = firstResponse;
    }

}
