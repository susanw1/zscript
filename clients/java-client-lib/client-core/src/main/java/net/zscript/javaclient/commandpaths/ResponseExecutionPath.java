package net.zscript.javaclient.commandpaths;

import javax.annotation.Nonnull;
import javax.annotation.Nullable;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.ListIterator;
import java.util.Optional;
import java.util.function.Consumer;

import static java.lang.Byte.toUnsignedInt;
import static java.lang.Integer.toHexString;
import static net.zscript.tokenizer.TokenBuffer.TokenReader.ReadToken;

import net.zscript.javaclient.ZscriptParseException;
import net.zscript.model.components.Zchars;
import net.zscript.tokenizer.TokenBufferIterator;
import net.zscript.tokenizer.Tokenizer;
import net.zscript.util.ByteString.ByteAppendable;
import net.zscript.util.ByteString.ByteStringBuilder;

/**
 * A parseable, unaddressed response sequence, without response value ('!')  or echo ('_') fields. It is essentially a chain of {@link ResponseElement} objects which can be matched
 * by a CommandExecutionPath (see {@link CommandExecutionPath#compareResponses(ResponseExecutionPath)}) to produce a complete comprehension of what happened during execution.
 * <p>
 * This class represents the result of execution of a {@link CommandExecutionPath} once the addressing and sequence-level fields have been processed, ie by a client runtime
 * representing a device node hierarchy, see {@link net.zscript.javaclient.nodes.ZscriptNode#send(CommandExecutionPath, Consumer)}.
 */
public class ResponseExecutionPath implements Iterable<ResponseElement>, ByteAppendable {

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
                    if (token.getKey() != Tokenizer.NORMAL_SEQUENCE_END) {
                        throw new ZscriptParseException("Syntax error [marker=%s, token=%s]", token, start);
                    }
                    break;
                } else {
                    throw new ZscriptParseException("Unknown separator [key=%s, token=%s]", toHexString(toUnsignedInt(token.getKey())), token);
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

        ResponseElement prev               = null;
        boolean         hasUnexplainedFail = false;
        int             bracketCount       = 0;
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

    private final ResponseElement firstResponse;

    private ResponseExecutionPath(@Nullable ResponseElement firstResponse) {
        this.firstResponse = firstResponse;
    }

    public ResponseElement getFirstResponse() {
        return firstResponse;
    }

    public List<ResponseElement> getResponses() {
        List<ResponseElement> resps = new ArrayList<>();
        for (ResponseElement r = firstResponse; r != null; r = r.getNext()) {
            resps.add(r);
        }
        return resps;
    }

    @Nonnull
    @Override
    public Iterator<ResponseElement> iterator() {
        return new Iterator<>() {
            ResponseElement r = firstResponse;

            @Override
            public boolean hasNext() {
                return r != null;
            }

            @Override
            public ResponseElement next() {
                ResponseElement tmp = r;
                r = r.getNext();
                return tmp;
            }
        };
    }

    @Override
    public void appendTo(ByteStringBuilder builder) {
        for (Iterator<ResponseElement> iter = iterator(); iter.hasNext(); ) {
            ResponseElement r = iter.next();
            r.appendTo(builder);
            if (iter.hasNext()) {
                r.separatorBytes(builder);
            }
        }
    }

    @Override
    public String toString() {
        return toStringImpl();
    }

    private static class ResponseBuilder {
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
        public ResponseElement generateResponse(@Nullable ResponseElement next, boolean unexplainedFail, int priorBrackets) {
            return new ResponseElement(next, !(unexplainedFail || separator == Zchars.Z_ORELSE), separator == '(', separator == ')', priorBrackets,
                    ZscriptFieldSet.fromTokens(start));
        }
    }
}
