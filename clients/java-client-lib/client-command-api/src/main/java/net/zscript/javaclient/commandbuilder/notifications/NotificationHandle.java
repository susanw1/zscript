package net.zscript.javaclient.commandbuilder.notifications;

import java.util.List;

import net.zscript.javaclient.commandbuilder.ZscriptResponse;

public abstract class NotificationHandle {
    public abstract <T extends ZscriptResponse> NotificationSection<T> getSection(NotificationSectionId<T> response);

    public abstract List<NotificationSection<?>> getSections();
}
