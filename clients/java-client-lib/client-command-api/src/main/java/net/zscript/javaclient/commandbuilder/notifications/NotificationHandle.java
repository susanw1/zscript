package net.zscript.javaclient.commandbuilder.notifications;

import net.zscript.javaclient.commandbuilder.ZscriptResponse;

public abstract class NotificationHandle {
    public abstract <T extends ZscriptResponse> NotificationSection<T> getSection(NotificationSectionId<T> response);
}
