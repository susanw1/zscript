package net.zscript.javaclient.responseparser;

import java.util.function.Consumer;

import net.zscript.javaclient.zscriptapi.CommandSeqElement;

public interface CommandResponseSystem {
    void send(CommandSeqElement seq);

    void send(ZscriptAddress addr, byte[] data);

    void send(byte[] zscript, Consumer<byte[]> callback);

    ResponseAddressingSystem getResponseAddressingSystem();
}
