package net.zscript.javaclient.commandbuilder.commandnodes;

import java.util.NoSuchElementException;

import net.zscript.javaclient.commandbuilder.ZscriptResponse;

public class ResponseCaptor<T extends ZscriptResponse> {
    public static <T extends ZscriptResponse> ResponseCaptor<T> create() {
        return new ResponseCaptor<>();
    }

    private ZscriptCommandNode<T> command;
    private T                     response = null;
    private boolean               called   = false;

    public T get() {
        if (called) {
            return response;
        } else {
            throw new NoSuchElementException("Command was not run, so no response exists");
        }
    }

    public boolean wasCalled() {
        return called;
    }

    public void setCommand(ZscriptCommandNode<T> command) {
        this.command = command;
    }

    public ZscriptCommandNode<T> getCommand() {
        return command;
    }

    public void resetResponseParsing() {
        called = false;
    }

    public void responseReceived(T response) {
        this.response = response;
        called = true;
    }
}
