package net.zscript.javareceiver.execution;

import net.zscript.javareceiver.core.SequenceOutStream;
import net.zscript.javareceiver.core.Zscript;
import net.zscript.javareceiver.notifications.ZscriptNotificationSource;
import net.zscript.javareceiver.semanticParser.ContextView.AsyncActionNotifier;

public class NotificationContext {
    private final ZscriptNotificationSource source;
    private final Zscript                   zscript;

    public NotificationContext(ZscriptNotificationSource source, Zscript zscript) {
        this.source = source;
        this.zscript = zscript;
        source.setNotificationComplete(true);
    }

    public void notificationComplete() {
        source.setNotificationComplete(true);
    }

    public void notificationNotComplete() {
        source.setNotificationComplete(false);
    }

    public boolean isNotificationComplete() {
        return source.isNotificationComplete();
    }

    public boolean isAddressing() {
        return source.isAddressing();
    }

    public int getID() {
        return source.getID();
    }

    public Zscript getZscript() {
        return zscript;
    }

    public SequenceOutStream getOutStream() {
        return zscript.getNotificationOutStream();
    }

    public AsyncActionNotifier getAsyncActionNotifier() {
        return source.getAsyncActionNotifier();
    }

}
