package net.zscript.javareceiver.demoRun2;

import net.zscript.javareceiver.tokenizer.TokenRingBuffer;
import net.zscript.javareceiver.tokenizer.Tokenizer;
import net.zscript.javareceiver.treeParser.ZscriptTreeParser;

public class Main {
    public static void main(String[] args) {
        TokenRingBuffer buff = TokenRingBuffer.createBufferWithCapacity(100);
        Tokenizer       in   = new Tokenizer(buff.getTokenWriter(), 2);
        for (byte b : "A(Z0)&D1|(B0)\n".getBytes()) {
            in.accept(b);
        }
        System.out.println(ZscriptTreeParser.parseAll(buff.getTokenReader().getFirstReadToken()));
    }
}
