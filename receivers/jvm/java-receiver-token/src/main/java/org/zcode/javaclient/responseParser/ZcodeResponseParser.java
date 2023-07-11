package org.zcode.javaclient.responseParser;

import java.util.ArrayList;
import java.util.List;
import java.util.Optional;

import org.zcode.javaclient.zcodeApi.CommandSeqElement;
import org.zcode.javaclient.zcodeApi.ZcodeCommand;
import org.zcode.javaclient.zcodeApi.ZcodeCommand.ZcodeSequencePath;
import org.zcode.javaclient.zcodeApi.ZcodeUnparsedCommandResponse;
import org.zcode.javareceiver.tokenizer.OptIterator;
import org.zcode.javareceiver.tokenizer.Zchars;
import org.zcode.javareceiver.tokenizer.ZcodeTokenBuffer;
import org.zcode.javareceiver.tokenizer.ZcodeTokenBuffer.TokenReader;
import org.zcode.javareceiver.tokenizer.ZcodeTokenBuffer.TokenReader.ReadToken;
import org.zcode.javareceiver.tokenizer.ZcodeTokenExtendingBuffer;
import org.zcode.javareceiver.tokenizer.ZcodeTokenizer;

public class ZcodeResponseParser {
    static class ResponseHeader {
        private final int type;
        private final int echo;

        public ResponseHeader(int type, int echo) {
            this.type = type;
            this.echo = echo;
        }

        public int getType() {
            return type;
        }

        public int getEcho() {
            return echo;
        }

    }

    public static ResponseHeader parseResponseHeader(byte[] resp) {
        int     respType = -1;
        int     echo     = -1;
        boolean inEcho   = false;
        if (resp.length == 0 || resp[0] != '!') {
            throw new IllegalArgumentException("Not a value response");
        }
        for (byte b : resp) {
            int num = -1;
            if (Zchars.shouldIgnore(b)) {
                continue;
            } else if (b == '!') {
                respType = 0;
            } else if (b == '_') {
                inEcho = true;
                echo = 0;
            } else if ('0' <= b && b <= '9') {
                num = b - '0';
            } else if ('a' <= b && b <= 'f') {
                num = b - 'a' + 10;
            } else {
                break;
            }
            if (num != -1) {
                if (inEcho) {
                    echo <<= 4;
                    echo |= num;
                } else {
                    respType <<= 4;
                    respType |= num;
                }
            }
        }
        return new ResponseHeader(respType, echo);
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

    public static void parseFullResponse(final CommandSeqElement command, final byte[] responce) {
        final ZcodeTokenBuffer buffer = new ZcodeTokenExtendingBuffer();
        final ZcodeTokenizer   in     = new ZcodeTokenizer(buffer.getTokenWriter(), 4);
        System.out.print("resp: ");
        for (final byte b : responce) {
            in.accept(b);
            System.out.print((char) b);
        }
        System.out.println();
        final TokenReader     reader            = buffer.getTokenReader();
        final List<Byte>      markers           = new ArrayList<>();
        final List<ReadToken> tokenAfterMarkers = new ArrayList<>();
        boolean               prevWasMarker     = false;
        tokenAfterMarkers.add(reader.getFirstReadToken());

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
        ZcodeCommand      current     = null;
        ZcodeSequencePath successPath = ZcodeCommand.findFirstCommand(command);
        ZcodeSequencePath failPath    = ZcodeCommand.findFirstCommand(command);

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
                    return;
                }
            }
            if (current == null) {
                break;
            }
            successPath = current.findSuccessPath();
            failPath = current.findFailPath();
            current.response(new ZcodeUnparsedCommandResponse(tokenAfterMarkers.get(offset)));
        }
    }
}
