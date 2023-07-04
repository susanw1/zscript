package org.zcode.javareceiver.demoRun;

import org.zcode.javareceiver.core.ZcodeChannel;
import org.zcode.javareceiver.execution.ZcodeExecutor;
import org.zcode.javareceiver.modules.ZcodeCommandFinder;
import org.zcode.javareceiver.modules.core.ZcodeCoreModule;
import org.zcode.javareceiver.semanticParser.SemanticParser;
import org.zcode.javareceiver.tokenizer.ZcodeTokenRingBuffer;
import org.zcode.javareceiver.tokenizer.ZcodeTokenizer;

public class Main {
    public static void main(String[] args) {
        ZcodeCommandFinder.addModule(new ZcodeCoreModule());
        String               code     = "Z0&Z1&Z2\n";
        ZcodeExecutor        executor = new ZcodeExecutor();
        ZcodeTokenRingBuffer rbuff    = ZcodeTokenRingBuffer.createBufferWithCapacity(100);
        ZcodeTokenizer       in       = new ZcodeTokenizer(rbuff.getTokenWriter(), 2);
        executor.addChannel(new ZcodeChannel(new SemanticParser(rbuff.getTokenReader()), new ZcodePrintingOutStream()));
        for (byte b : code.getBytes()) {
            in.accept(b);
            executor.progress();
        }
        for (int i = 0; i < 1000; i++) {
            executor.progress();
        }
        while (true) {
            executor.progress();
        }
    }
}
