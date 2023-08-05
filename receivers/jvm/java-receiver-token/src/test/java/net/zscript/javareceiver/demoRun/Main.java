package net.zscript.javareceiver.demoRun;

import java.io.IOException;

import net.zscript.javareceiver.core.OutStream;
import net.zscript.javareceiver.core.Zscript;
import net.zscript.javareceiver.modules.core.ZscriptCoreModule;
import net.zscript.javareceiver.modules.outerCore.ZscriptOuterCoreModule;
import net.zscript.javareceiver.scriptSpaces.ScriptSpace;

public class Main {
    public static void main(String[] args) throws IOException {
        Zscript                zscript = new Zscript();
        ZscriptOuterCoreModule oCore   = new ZscriptOuterCoreModule();
        zscript.addNotificationSource(oCore.getNotificationSource());
        zscript.addModule(oCore);
        zscript.addModule(new ZscriptCoreModule());
//        byte[] code = "Z2(Z1S1)Z1&Z1|Z1\n".getBytes();
        zscript.addActionSource(ScriptSpace.from(zscript, "Z0&Z1S1\nZ2\nZ1S10\n"));

//        TokenRingBuffer rbuff = TokenRingBuffer.createBufferWithCapacity(100);
        OutStream out = new ZscriptPrintingOutStream();
//        zscript.addChannel(new ZscriptChannel(rbuff, out) {
//            Tokenizer   in = new Tokenizer(rbuff.getTokenWriter(), 2);
//            private int i  = 0;
//
//            @Override
//            public void moveAlong() {
//                if (i < code.length) {
//                    in.accept(code[i++]);
//                }
//            }
//
//            @Override
//            public void channelInfo(CommandContext ctx) {
//                ctx.getOutStream().writeQuotedString("Basic text channel");
//            }
//
//            @Override
//            public void channelSetup(CommandContext ctx) {
//            }
//        });
        zscript.setNotificationOutStream(out);
        for (int i = 0; i < 10000; i++) {
            zscript.progress();
        }
    }
}
