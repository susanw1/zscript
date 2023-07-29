package org.zcode.javareceiver.demoRun;

import org.zcode.javareceiver.core.Zcode;
import org.zcode.javareceiver.core.ZcodeChannel;
import org.zcode.javareceiver.core.ZcodeOutStream;
import org.zcode.javareceiver.execution.ZcodeCommandContext;
import org.zcode.javareceiver.modules.core.ZcodeCoreModule;
import org.zcode.javareceiver.modules.outerCore.ZcodeOuterCoreModule;
import org.zcode.javareceiver.tokenizer.ZcodeTokenRingBuffer;
import org.zcode.javareceiver.tokenizer.ZcodeTokenizer;

public class Main {
    public static void main(String[] args) {
        Zcode                zcode = new Zcode();
        ZcodeOuterCoreModule oCore = new ZcodeOuterCoreModule();
        zcode.addNotificationSource(oCore.getNotificationSource());
        zcode.addModule(oCore);
        zcode.addModule(new ZcodeCoreModule());
        byte[] code = "Z2(Z1S1)Z1&Z1|Z1\n".getBytes();

        ZcodeTokenRingBuffer rbuff = ZcodeTokenRingBuffer.createBufferWithCapacity(100);
        ZcodeOutStream       out   = new ZcodePrintingOutStream();
        zcode.addChannel(new ZcodeChannel(rbuff, out) {
            ZcodeTokenizer in = new ZcodeTokenizer(rbuff.getTokenWriter(), 2);
            private int    i  = 0;

            @Override
            public void moveAlong() {
                if (i < code.length) {
                    in.accept(code[i++]);
                }
            }

            @Override
            public void channelInfo(ZcodeCommandContext ctx) {
                ctx.getOutStream().writeString("Basic text channel");
            }

            @Override
            public void channelSetup(ZcodeCommandContext ctx) {
            }
        });
        zcode.setNotificationOutStream(out);
        for (int i = 0; i < 10000; i++) {
            zcode.progress();
        }
    }
}
