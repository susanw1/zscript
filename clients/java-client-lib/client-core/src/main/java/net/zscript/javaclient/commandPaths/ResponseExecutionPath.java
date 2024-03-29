package net.zscript.javaclient.commandPaths;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.ListIterator;
import java.util.Optional;

import static net.zscript.javareceiver.tokenizer.TokenBuffer.TokenReader.ReadToken;


import net.zscript.javaclient.ZscriptByteString;
import net.zscript.javareceiver.tokenizer.TokenBufferIterator;
import net.zscript.javareceiver.tokenizer.Tokenizer;
import net.zscript.model.components.Zchars;
import net.zscript.util.ByteString;

public class ResponseExecutionPath implements Iterable<Response> {

    static class ResponseBuilder {
        ReadToken start = null;

        byte seperator = 0;

        public ReadToken getStart() {
            return start;
        }

        public void setStart(ReadToken token) {
            this.start = token;
        }

        public boolean isEmpty() {
            return start.isMarker();
        }

        public boolean isUnexplainedFail() {
            return seperator == Zchars.Z_ORELSE && isEmpty();
        }

        public Response generateResponse(Response next, boolean unexplainedFail, int priorBrackets) {
            return new Response(next, !(unexplainedFail || seperator == Zchars.Z_ORELSE), seperator == '(', seperator == ')', priorBrackets, ZscriptFieldSet.fromTokens(start));
        }
    }

    private static List<ResponseBuilder> createLinkedPath(ReadToken start) {
        List<ResponseBuilder> builders = new ArrayList<>();

        ResponseBuilder last = new ResponseBuilder();
        builders.add(last);
        if (start == null) {
            return builders;
        }
        TokenBufferIterator iterator = start.getNextTokens();
        for (Optional<ReadToken> opt = iterator.next(); opt.isPresent(); opt = iterator.next()) {
            ReadToken token = opt.get();
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

    public static ResponseExecutionPath blank() {
        return new ResponseExecutionPath(null);
    }
    public static ResponseExecutionPath parse(ReadToken start) {
        List<ResponseBuilder> builders = createLinkedPath(start);

        Response prev               = null;
        boolean  hasUnexplainedFail = false;
        int      bracketCount       = 0;
        for (ListIterator<ResponseBuilder> iter = builders.listIterator(builders.size()); iter.hasPrevious(); ) {
            ResponseBuilder b = iter.previous();
            if (b.start != null) {
                if (hasUnexplainedFail) {
                    if (!b.isEmpty()) {
                        prev = b.generateResponse(prev, hasUnexplainedFail, bracketCount);
                        hasUnexplainedFail = false;
                        bracketCount = 0;
                    }
                } else {
                    if (b.isUnexplainedFail()) {
                        bracketCount++;
                        hasUnexplainedFail = true;
                    } else {
                        prev = b.generateResponse(prev, hasUnexplainedFail, 0);
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

    public Iterator<Response> iterator() {
        return new Iterator<Response>() {
            Response r = firstResponse;

            @Override
            public boolean hasNext() {
                return r != null;
            }

            @Override
            public Response next() {
                Response tmp = r;
                r = r.getNext();
                return tmp;
            }
        };
    }

    public ByteString toSequence() {
        ZscriptByteString.ZscriptByteStringBuilder out = ZscriptByteString.builder();
        toSequence(out);
        return out.build();
    }

    public void toSequence(ZscriptByteString.ZscriptByteStringBuilder out) {
        for (Iterator<Response> iter = iterator(); iter.hasNext(); ) {
            Response r = iter.next();
            r.toBytes(out);
            if (iter.hasNext()) {
                r.seperatorBytes(out);
            }
        }
    }

}
