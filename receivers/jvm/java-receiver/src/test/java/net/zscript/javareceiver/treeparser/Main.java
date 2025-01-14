package net.zscript.javareceiver.treeparser;

import net.zscript.tokenizer.TokenRingBuffer;
import net.zscript.tokenizer.Tokenizer;

public class Main {
    public static void main(String[] args) {
        TokenRingBuffer buff = TokenRingBuffer.createBufferWithCapacity(100);
        Tokenizer       in   = new Tokenizer(buff.getTokenWriter(), false);
        for (byte b : "A(Z0)&D1|(B0)\n".getBytes()) {
            in.accept(b);
        }
        System.out.println(ZscriptTreeParser.parseAll(buff.getTokenReader().getFirstReadToken()));
    }
}
