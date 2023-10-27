package net.zscript.javaclient.commandbuilder.commandnodes;

import java.util.NoSuchElementException;

import net.zscript.javaclient.commandbuilder.ZscriptResponse;

public class ResponseCaptor<T extends ZscriptResponse> {
    public static <T extends ZscriptResponse> ResponseCaptor<T> create() {
        return new ResponseCaptor<>();
    }

    private ZscriptCommandNode<T> command;

    public void setCommand(ZscriptCommandNode<T> command) {
        this.command = command;
    }

    public ZscriptCommandNode<T> getCommand() {
        return command;
    }
}
