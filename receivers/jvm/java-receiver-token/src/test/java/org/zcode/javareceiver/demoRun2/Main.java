package org.zcode.javareceiver.demoRun2;

import org.zcode.javareceiver.tokenizer.ZcodeTokenRingBuffer;
import org.zcode.javareceiver.tokenizer.ZcodeTokenizer;
import org.zcode.javareceiver.treeParser.ZcodeTreeParser;

public class Main {
    public static void main(String[] args) {
        ZcodeTokenRingBuffer buff = ZcodeTokenRingBuffer.createBufferWithCapacity(100);
        ZcodeTokenizer       in   = new ZcodeTokenizer(buff.getTokenWriter(), 2);
        for (byte b : "A(Z0)&D1|(B0)\n".getBytes()) {
            in.accept(b);
        }
        System.out.println(ZcodeTreeParser.parseAll(buff.getTokenReader().getFirstReadToken()));
    }
}
