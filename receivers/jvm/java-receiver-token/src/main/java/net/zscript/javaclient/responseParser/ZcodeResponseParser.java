package net.zscript.javaclient.responseParser;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Optional;
import java.util.Set;

import net.zscript.javaclient.zcodeApi.CommandSeqElement;
import net.zscript.javaclient.zcodeApi.ZcodeCommand;
import net.zscript.javaclient.zcodeApi.ZcodeUnparsedCommandResponse;
import net.zscript.javaclient.zcodeApi.ZcodeCommand.ZcodeSequencePath;
import net.zscript.javareceiver.tokenizer.OptIterator;
import net.zscript.javareceiver.tokenizer.ZcodeTokenBuffer;
import net.zscript.javareceiver.tokenizer.ZcodeTokenExtendingBuffer;
import net.zscript.javareceiver.tokenizer.ZcodeTokenizer;
import net.zscript.javareceiver.tokenizer.ZcodeTokenBuffer.TokenReader;
import net.zscript.javareceiver.tokenizer.ZcodeTokenBuffer.TokenReader.ReadToken;

public class ZcodeResponseParser {
    static class ResponseHeader {
        private final int[] addr;
        private final int   type;
        private final int   echo;

        public ResponseHeader(int[] addr, int type, int echo) {
            this.addr = addr;
            this.type = type;
            this.echo = echo;
        }

        public int[] getAddr() {
            return addr;
        }

        public int getType() {
            return type;
        }

        public int getEcho() {
            return echo;
        }

    }

    public static ResponseHeader parseResponseHeader(byte[] resp) {
        final ZcodeTokenBuffer buffer = new ZcodeTokenExtendingBuffer();
        final ZcodeTokenizer   in     = new ZcodeTokenizer(buffer.getTokenWriter(), 4);
        final TokenReader      reader = buffer.getTokenReader();

        ReadToken lastWritten = null;
        for (final byte b : resp) {
            in.accept(b);
            if (lastWritten == null) {
                if (reader.hasReadToken()) {
                    lastWritten = reader.getFirstReadToken();
                }
            } else {
                Optional<ReadToken> opt = lastWritten.getNextTokens().next();
                if (opt.isPresent()) {
                    lastWritten = opt.get();
                }
                if (lastWritten.getKey() != '_' && lastWritten.getKey() != '!' && lastWritten.getKey() != '@' && lastWritten.getKey() != '.') {
                    break;
                }
            }
            if (buffer.getTokenReader().getFlags().getAndClearMarkerWritten()) {
                break;
            }
        }
        int respType = -1;
        int echo     = -1;

        List<Integer> addr = new ArrayList<>();

        OptIterator<ReadToken> it = reader.iterator();
        for (Optional<ReadToken> opt = it.next(); opt.isPresent(); opt = it.next()) {
            ReadToken token = opt.get();
            if (token.getKey() == '_') {
                echo = token.getData16();
            } else if (token.getKey() == '!') {
                respType = token.getData16();
            } else if (token.getKey() == '@' || token.getKey() == '.') {
                addr.add(token.getData16());
            } else {
                break;
            }
        }
        int[] addrArr = new int[addr.size()];
        int   i       = 0;
        for (int val : addr) {
            addrArr[i++] = val;
        }
        return new ResponseHeader(addrArr, respType, echo);
    }

    private static byte convertMarkers(byte encoded) {
        if (encoded == ZcodeTokenizer.CMD_END_ANDTHEN) {
            return '&';
        } else if (encoded == ZcodeTokenizer.CMD_END_ORELSE) {
            return '|';
        } else if (encoded == ZcodeTokenizer.CMD_END_OPEN_PAREN) {
            return '(';
        } else if (encoded == ZcodeTokenizer.CMD_END_CLOSE_PAREN) {
            return ')';
        } else if (encoded == ZcodeTokenizer.NORMAL_SEQUENCE_END) {
            return '\n';
        } else {
            throw new IllegalArgumentException("Unknown marker: " + Integer.toHexString(Byte.toUnsignedInt(encoded)));
        }
    }

    // TODO: Trim the sequence level stuff off of first command
    public static void parseFullResponse(final CommandSeqElement command, final byte[] responce) {
        final ZcodeTokenBuffer buffer = new ZcodeTokenExtendingBuffer();
        final ZcodeTokenizer   in     = new ZcodeTokenizer(buffer.getTokenWriter(), 4);
        for (final byte b : responce) {
            in.accept(b);
        }
        final TokenReader     reader            = buffer.getTokenReader();
        final List<Byte>      markers           = new ArrayList<>();
        final List<ReadToken> tokenAfterMarkers = new ArrayList<>();
        boolean               prevWasMarker     = false;

        final OptIterator<ReadToken> itEndSeq = reader.iterator();
        for (Optional<ReadToken> opt = itEndSeq.next(); opt.isPresent(); opt = itEndSeq.next()) {
            if (opt.get().getKey() != '!' && opt.get().getKey() != '_') {
                tokenAfterMarkers.add(opt.get());
                break;
            }
        }

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
        ZcodeCommand      current       = null;
        ZcodeSequencePath successPath   = ZcodeCommand.findFirstCommand(command);
        ZcodeSequencePath failPath      = ZcodeCommand.findFirstCommand(command);
        Set<ZcodeCommand> sentResponses = new HashSet<>();

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
//                System.out.println("Success: " + successPath.getMarkers());
            }
//            if (failPath != null) {
//                System.out.println("Fail: " + failPath.getMarkers());
//            }
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
                if (canBeFail) {
                    offset += failPath.getMarkers().size();
                    current = failPath.getNext();
                } else {
                    break;
                }
            }
            if (current == null) {
                break;
            }
            successPath = current.findSuccessPath();
            failPath = current.findFailPath();
            current.response(new ZcodeUnparsedCommandResponse(tokenAfterMarkers.get(offset)));
            sentResponses.add(current);
        }
        for (ZcodeSequencePath path = ZcodeCommand.findFirstCommand(command); path != null; path = path.getNext().findNext()) {
            ZcodeCommand cmd = path.getNext();
            if (!sentResponses.contains(cmd)) {
                cmd.notExecuted();
            }
        }
    }

}
