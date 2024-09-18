package net.zscript.javaclient.commandbuilder.commandnodes;

import javax.annotation.Nonnull;
import javax.annotation.Nullable;

import net.zscript.javaclient.commandbuilder.Respondable;
import net.zscript.javaclient.commandbuilder.ZscriptResponse;

public class ResponseCaptor<T extends ZscriptResponse> {
    @Nonnull
    public static <T extends ZscriptResponse> ResponseCaptor<T> create() {
        return new ResponseCaptor<>();
    }

    private ResponseCaptor() {
    }

    private Respondable<T> source;

    public void setSource(Respondable<T> source) {
        this.source = source;
    }

    @Nullable
    public Respondable<T> getSource() {
        return source;
    }
}
