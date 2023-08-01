package net.zscript.javaclient.responseParser;

import java.util.function.Consumer;

import net.zscript.javaclient.zcodeApi.CommandSeqElement;

public interface CommandResponseSystem {
    void send(CommandSeqElement seq);

    void send(ZscriptAddress addr, byte[] data);

    void send(byte[] zcode, Consumer<byte[]> callback);

    ResponseAddressingSystem getResponseAddressingSystem();
}
