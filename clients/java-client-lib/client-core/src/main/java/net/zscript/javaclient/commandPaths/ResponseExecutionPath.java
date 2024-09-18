package net.zscript.javaclient.commandPaths;

import javax.annotation.Nonnull;
import javax.annotation.Nullable;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.ListIterator;
import java.util.Optional;

import static net.zscript.tokenizer.TokenBuffer.TokenReader.ReadToken;

import net.zscript.model.components.Zchars;
import net.zscript.tokenizer.TokenBufferIterator;
import net.zscript.tokenizer.Tokenizer;
import net.zscript.util.ByteString.ByteAppendable;
import net.zscript.util.ByteString.ByteStringBuilder;

public class ResponseExecutionPath implements Iterable<Response>, ByteAppendable {

    static class ResponseBuilder {
        ReadToken start = null;

        byte separator = 0;

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
            return separator == Zchars.Z_ORELSE && isEmpty();
        }

        @Nonnull
        public Response generateResponse(@Nullable Response next, boolean unexplainedFail, int priorBrackets) {
            return new Response(next, !(unexplainedFail || separator == Zchars.Z_ORELSE), separator == '(', separator == ')', priorBrackets, ZscriptFieldSet.fromTokens(start));
        }
    }

    @Nonnull
    private static List<ResponseBuilder> createLinkedPath(@Nullable ReadToken start) {
        List<ResponseBuilder> builders = new ArrayList<>();

        ResponseBuilder last = new ResponseBuilder();
        builders.add(last);
        if (start == null) {
            return builders;
        }
        TokenBufferIterator iterator = start.tokenIterator();
        for (Optional<ReadToken> opt = iterator.next(); opt.isPresent(); opt = iterator.next()) {
            ReadToken token = opt.get();
            if (last.getStart() == null) {
                last.setStart(token);
            }
            if (token.isMarker()) {
                ResponseBuilder next = new ResponseBuilder();
                builders.add(next);
                if (token.getKey() == Tokenizer.CMD_END_ANDTHEN) {
                    last.separator = Zchars.Z_ANDTHEN;
                } else if (token.getKey() == Tokenizer.CMD_END_ORELSE) {
                    last.separator = Zchars.Z_ORELSE;
                } else if (token.getKey() == Tokenizer.CMD_END_OPEN_PAREN) {
                    last.separator = Zchars.Z_OPENPAREN;
                } else if (token.getKey() == Tokenizer.CMD_END_CLOSE_PAREN) {
                    last.separator = Zchars.Z_CLOSEPAREN;
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

    @Nonnull
    public static ResponseExecutionPath blank() {
        return new ResponseExecutionPath(null);
    }

    @Nonnull
    public static ResponseExecutionPath parse(@Nullable ReadToken start) {
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

    private ResponseExecutionPath(@Nullable Response firstResponse) {
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

    @Nonnull
    @Override
    public Iterator<Response> iterator() {
        return new Iterator<>() {
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

    @Override
    public void appendTo(ByteStringBuilder builder) {
        for (Iterator<Response> iter = iterator(); iter.hasNext(); ) {
            Response r = iter.next();
            r.appendTo(builder);
            if (iter.hasNext()) {
                r.separatorBytes(builder);
            }
        }
    }

}
