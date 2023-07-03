package org.zcode.javareceiver.modules.core;

import java.util.Iterator;
import java.util.Optional;

import org.zcode.javareceiver.core.ZcodeOutStream;
import org.zcode.javareceiver.execution.ZcodeCommandEnviroment;
import org.zcode.javareceiver.tokenizer.OptIterator;
import org.zcode.javareceiver.tokenizer.ZcodeTokenBuffer.TokenReader.ReadToken;

public class ZcodeEchoCommand {

    public static void execute(ZcodeCommandEnviroment env) {
        ZcodeOutStream         out      = env.getOutStream();
        OptIterator<ReadToken> iterator = env.tokenIterator();
        for (Optional<ReadToken> opt = iterator.next(); opt.isPresent(); opt = iterator.next()) {
            ReadToken token = opt.get();
            if (token.getKey() == 'S') {
                int status = token.getData16();
                env.status((byte) status);
                continue;
            }
            out.startField(token.getKey());
            for (Iterator<Byte> it = token.blockIterator(); it.hasNext();) {
                out.continueField(it.next());
            }
            if (token.getKey() == '"') {
                out.writeByte((byte) '"');
            }
        }
    }

}
