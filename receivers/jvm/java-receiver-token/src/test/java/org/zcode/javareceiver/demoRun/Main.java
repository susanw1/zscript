package org.zcode.javareceiver.demoRun;

import org.zcode.javareceiver.core.Zcode;
import org.zcode.javareceiver.core.ZcodeChannel;
import org.zcode.javareceiver.modules.ZcodeCommandFinder;
import org.zcode.javareceiver.modules.core.ZcodeCoreModule;
import org.zcode.javareceiver.modules.outerCore.ZcodeOuterCoreModule;
import org.zcode.javareceiver.tokenizer.ZcodeTokenRingBuffer;
import org.zcode.javareceiver.tokenizer.ZcodeTokenizer;

public class Main {
    public static void main(String[] args) {
        ZcodeCommandFinder.addModule(new ZcodeCoreModule());
        ZcodeCommandFinder.addModule(new ZcodeOuterCoreModule());
        byte[] code = "(Z1S10)Z1&Z1|Z1)\n".getBytes();

        Zcode                zcode = new Zcode();
        ZcodeTokenRingBuffer rbuff = ZcodeTokenRingBuffer.createBufferWithCapacity(100);
        zcode.addChannel(new ZcodeChannel(rbuff, new ZcodePrintingOutStream()) {
            ZcodeTokenizer in = new ZcodeTokenizer(rbuff.getTokenWriter(), 2);
            private int    i  = 0;

            @Override
            public void moveAlong() {
                if (i < code.length) {
                    in.accept(code[i++]);
                }
            }
        });
        for (int i = 0; i < 10000; i++) {
            zcode.progress();
        }
    }
}
