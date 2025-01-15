package net.zscript.javaclient.commandbuilderapi.notifications;

import net.zscript.javaclient.commandbuilderapi.Respondable;
import net.zscript.javaclient.commandbuilderapi.ZscriptResponse;
import net.zscript.javaclient.commandbuilderapi.nodes.ResponseCaptor;
import net.zscript.tokenizer.ZscriptExpression;

public abstract class NotificationSection<T extends ZscriptResponse> implements Respondable<T> {
    public abstract Class<T> getResponseType();

    public void setCaptor(ResponseCaptor<T> captor) {
        captor.setSource(this);
    }

    public abstract T parseResponse(ZscriptExpression expression);
}
