package net.zscript.javaclient.commandbuilder.notifications;

import javax.annotation.Nonnull;
import java.util.List;

import net.zscript.javaclient.commandbuilder.ZscriptResponse;

public abstract class NotificationHandle {
    @Nonnull
    public abstract <T extends ZscriptResponse> NotificationSection<T> getSection(NotificationSectionId<T> response);

    @Nonnull
    public abstract List<NotificationSection<?>> getSections();
}
