package net.zscript.javaclient.responseParser;

import java.util.function.Consumer;

import net.zscript.javaclient.zcodeApi.CommandSeqElement;

public interface ZcodeCommandResponseSystem {
    void send(CommandSeqElement seq);

    void send(ZcodeAddress addr, byte[] data);

    void send(byte[] zcode, Consumer<byte[]> callback);

    ZcodeResponseAddressingSystem getResponseAddressingSystem();
}
