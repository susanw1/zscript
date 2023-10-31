package net.zscript.javaclient.commandbuilder.commandnodes;

import java.util.NoSuchElementException;

import net.zscript.javaclient.commandbuilder.Respondable;
import net.zscript.javaclient.commandbuilder.ZscriptResponse;

public class ResponseCaptor<T extends ZscriptResponse> {
    public static <T extends ZscriptResponse> ResponseCaptor<T> create() {
        return new ResponseCaptor<>();
    }

    private Respondable<T> source;

    public void setSource(Respondable<T> source) {
        this.source = source;
    }

    public Respondable<T> getSource() {
        return source;
    }
}
