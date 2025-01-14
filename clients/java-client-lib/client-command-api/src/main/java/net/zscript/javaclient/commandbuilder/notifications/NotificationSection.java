package net.zscript.javaclient.commandbuilder.notifications;

import net.zscript.javaclient.commandbuilder.Respondable;
import net.zscript.javaclient.commandbuilder.ZscriptResponse;
import net.zscript.javaclient.commandbuilder.nodes.ResponseCaptor;
import net.zscript.tokenizer.ZscriptExpression;

public abstract class NotificationSection<T extends ZscriptResponse> implements Respondable<T> {
    public abstract Class<T> getResponseType();

    public void setCaptor(ResponseCaptor<T> captor) {
        captor.setSource(this);
    }

    public abstract T parseResponse(ZscriptExpression expression);
}
