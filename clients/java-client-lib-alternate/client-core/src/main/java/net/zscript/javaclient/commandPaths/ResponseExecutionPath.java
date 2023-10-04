package net.zscript.javaclient.commandPaths;

import java.util.ArrayList;
import java.util.List;
import java.util.ListIterator;
import java.util.Optional;

import net.zscript.javareceiver.tokenizer.TokenBuffer;
import net.zscript.javareceiver.tokenizer.TokenBufferIterator;
import net.zscript.javareceiver.tokenizer.Tokenizer;
import net.zscript.model.components.Zchars;

public class ResponseExecutionPath {

    static class ResponseBuilder {
        TokenBuffer.TokenReader.ReadToken start = null;

        byte seperator = 0;

        public TokenBuffer.TokenReader.ReadToken getStart() {
            return start;
        }

        public void setStart(TokenBuffer.TokenReader.ReadToken token) {
            this.start = token;
        }

        public boolean isEmpty() {
            return start.isMarker();
        }

        public boolean isUnexplainedFail() {
            return seperator == Zchars.Z_ORELSE && isEmpty();
        }

        public Response generateResponse(Response next, boolean unexplainedFail) {
            return new Response(next, !(unexplainedFail || seperator == Zchars.Z_ORELSE), ZscriptFieldSet.fromTokens(start));
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
                last = next;
            }
        }
        return builders;
    }

    public static ResponseExecutionPath parse(TokenBuffer.TokenReader.ReadToken start) {
        List<ResponseBuilder> builders = createLinkedPath(start);

        Response first = null;

        Response prev               = null;
        boolean  hasUnexplainedFail = false;
        for (ListIterator<ResponseBuilder> iter = builders.listIterator(builders.size()); iter.hasPrevious(); ) {
            ResponseBuilder b = iter.previous();
            if (b.start != null) {
                if (hasUnexplainedFail) {
                    if (!b.isEmpty()) {
                        prev = b.generateResponse(prev, hasUnexplainedFail);
                        hasUnexplainedFail = false;
                    }
                } else {
                    if (b.isUnexplainedFail()) {
                        hasUnexplainedFail = true;
                    } else {
                        prev = b.generateResponse(prev, hasUnexplainedFail);
                    }
                }
            }
        }
        return new ResponseExecutionPath(prev);
    }

    private final Response firstResponse;

    private ResponseExecutionPath(Response firstResponse) {
        this.firstResponse = firstResponse;
    }

    public Response getFirstResponse() {
        return firstResponse;
    }

    public List<Response> getResponses() {
        List<Response> resps = new ArrayList<>();
        for (Response r = firstResponse; r != null; r = r.getNext()) {
            resps.add(r);
        }
        return resps;
    }
}
