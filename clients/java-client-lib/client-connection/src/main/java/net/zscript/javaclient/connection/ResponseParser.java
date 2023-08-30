package net.zscript.javaclient.connection;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Optional;
import java.util.Set;

import net.zscript.javaclient.commandbuilder.CommandSeqElement;
import net.zscript.javaclient.commandbuilder.ZscriptCommand;
import net.zscript.javaclient.commandbuilder.ZscriptCommand.ZscriptSequencePath;
import net.zscript.javareceiver.tokenizer.TokenBuffer;
import net.zscript.javareceiver.tokenizer.TokenBuffer.TokenReader;
import net.zscript.javareceiver.tokenizer.TokenBuffer.TokenReader.ReadToken;
import net.zscript.javareceiver.tokenizer.TokenExtendingBuffer;
import net.zscript.javareceiver.tokenizer.Tokenizer;
import net.zscript.javareceiver.tokenizer.ZscriptTokenExpression;
import net.zscript.model.components.Zchars;
import net.zscript.util.OptIterator;

public class ResponseParser {
    static class ResponseHeader {
        private final ZscriptAddress address;
        private final int            type;
        private final int            echo;

        public ResponseHeader(ZscriptAddress address, int type, int echo) {
            this.address = address;
            this.type = type;
            this.echo = echo;
        }

        public ZscriptAddress getAddress() {
            return address;
        }

        public boolean hasAddress() {
            return address.size() > 0;
        }

        public int getType() {
            return type;
        }

        public int getEcho() {
            return echo;
        }

    }

    public static ResponseHeader parseResponseHeader(byte[] resp) {
        final TokenBuffer buffer = new TokenExtendingBuffer();
        final Tokenizer   in     = new Tokenizer(buffer.getTokenWriter(), 4);
        final TokenReader reader = buffer.getTokenReader();

        ReadToken lastWritten = null;
        for (final byte b : resp) {
            in.accept(b);
            if (lastWritten == null) {
                if (reader.hasReadToken()) {
                    lastWritten = reader.getFirstReadToken();
                }
            } else {
                final Optional<ReadToken> opt = lastWritten.getNextTokens().next();
                if (opt.isPresent()) {
                    lastWritten = opt.get();
                }
                if (lastWritten.getKey() != Zchars.Z_ECHO
                        && lastWritten.getKey() != Zchars.Z_RESPONSE_MARK
                        && lastWritten.getKey() != Zchars.Z_ADDRESSING
                        && lastWritten.getKey() != Zchars.Z_ADDRESSING_CONTINUE) {
                    break;
                }
            }
            if (buffer.getTokenReader().getFlags().getAndClearMarkerWritten()) {
                break;
            }
        }
        int respType = -1;
        int echo     = -1;

        final List<Integer> addr = new ArrayList<>();

        OptIterator<ReadToken> it = reader.iterator();
        for (Optional<ReadToken> opt = it.next(); opt.isPresent(); opt = it.next()) {
            final ReadToken token = opt.get();
            if (token.getKey() == Zchars.Z_ECHO) {
                echo = token.getData16();
            } else if (token.getKey() == Zchars.Z_RESPONSE_MARK) {
                respType = token.getData16();
            } else if (token.getKey() == Zchars.Z_ADDRESSING || token.getKey() == Zchars.Z_ADDRESSING_CONTINUE) {
                addr.add(token.getData16());
            } else {
                break;
            }
        }
        return new ResponseHeader(ZscriptAddress.from(addr), respType, echo);
    }

    private static byte convertMarkers(byte encoded) {
        if (encoded == Tokenizer.CMD_END_ANDTHEN) {
            return Zchars.Z_ANDTHEN;
        }
        if (encoded == Tokenizer.CMD_END_ORELSE) {
            return Zchars.Z_ORELSE;
        }
        if (encoded == Tokenizer.CMD_END_OPEN_PAREN) {
            return Zchars.Z_OPENPAREN;
        }
        if (encoded == Tokenizer.CMD_END_CLOSE_PAREN) {
            return Zchars.Z_CLOSEPAREN;
        }
        if (encoded == Tokenizer.NORMAL_SEQUENCE_END) {
            return Zchars.Z_NEWLINE;
        }
        throw new IllegalArgumentException("Unknown marker: " + Integer.toHexString(Byte.toUnsignedInt(encoded)));
    }

    // TODO: Trim the sequence level stuff off of first command
    public static void parseFullResponse(final CommandSeqElement command, final byte[] response) {
        final TokenBuffer buffer = new TokenExtendingBuffer();
        final Tokenizer   in     = new Tokenizer(buffer.getTokenWriter(), 4);
        for (final byte b : response) {
            in.accept(b);
        }
        final TokenReader     reader            = buffer.getTokenReader();
        final List<Byte>      markers           = new ArrayList<>();
        final List<ReadToken> tokenAfterMarkers = new ArrayList<>();
        boolean               prevWasMarker     = false;

        reader.iterator().stream()
                .filter(tok -> tok.getKey() != Zchars.Z_RESPONSE_MARK && tok.getKey() != Zchars.Z_ECHO)
                .findFirst()
                .ifPresent(tokenAfterMarkers::add);

        final OptIterator<ReadToken> it = reader.iterator();
        for (Optional<ReadToken> opt = it.next(); opt.isPresent(); opt = it.next()) {
            final ReadToken token = opt.get();
            if (prevWasMarker) {
                tokenAfterMarkers.add(token);
                prevWasMarker = false;
            }
            if (token.isMarker()) {
                markers.add(convertMarkers(token.getKey()));
                prevWasMarker = true;
            }
        }
        matchMarkers(command, markers, tokenAfterMarkers);
    }

    private static void matchMarkers(final CommandSeqElement command, final List<Byte> markers, final List<ReadToken> tokenAfterMarkers) {
        ZscriptCommand      current;
        ZscriptSequencePath successPath = ZscriptCommand.findFirstCommand(command);
        ZscriptSequencePath failPath    = ZscriptCommand.findFirstCommand(command);

        final Set<ZscriptCommand> sentResponses = new HashSet<>();

        int offset = 0;

        while (true) {
            boolean canBeSuccess = true;
            if (successPath == null) {
                canBeSuccess = false;
            } else {
                for (int i = 0; i < successPath.getMarkers().size(); i++) {
                    if (markers.get(offset + i) != successPath.getMarkers().get(i)) {
                        canBeSuccess = false;
                        break;
                    }
                }
            }
            if (canBeSuccess) {
                offset += successPath.getMarkers().size();
                current = successPath.getNext();
            } else {
                boolean canBeFail = true;
                if (failPath == null) {
                    canBeFail = false;
                } else {
                    for (int i = 0; i < failPath.getMarkers().size(); i++) {
                        if (markers.get(offset + i) != failPath.getMarkers().get(i)) {
                            canBeFail = false;
                            break;
                        }
                    }
                }
                if (!canBeFail) {
                    break;
                }
                offset += failPath.getMarkers().size();
                current = failPath.getNext();
            }
            if (current == null) {
                break;
            }
            successPath = current.findSuccessPath();
            failPath = current.findFailPath();

            final ReadToken t = tokenAfterMarkers.get(offset);
            current.onResponse(new ZscriptTokenExpression(t::getNextTokens));

            sentResponses.add(current);
        }
        for (ZscriptSequencePath path = ZscriptCommand.findFirstCommand(command); path != null; path = path.getNext().findNext()) {
            ZscriptCommand cmd = path.getNext();
            if (!sentResponses.contains(cmd)) {
                cmd.onNotExecuted();
            }
        }
    }
}
