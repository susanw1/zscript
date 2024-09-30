package net.zscript.javaclient.commandbuilder.notifications;

import javax.annotation.Nonnull;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import net.zscript.javaclient.commandPaths.Response;
import net.zscript.javaclient.commandPaths.ResponseExecutionPath;
import net.zscript.javaclient.commandbuilder.ZscriptClientException;
import net.zscript.javaclient.commandbuilder.ZscriptResponse;

public abstract class NotificationHandle {
    @Nonnull
    public abstract <T extends ZscriptResponse> NotificationSection<T> getSection(NotificationSectionId<T> response);

    @Nonnull
    @Deprecated
    public abstract List<NotificationSection<?>> getSections();

    /**
     * Creates a list of the right types of notification section content
     *
     * @param responsePath
     * @return
     */
    public List<ZscriptResponse> buildNotificationContent(final ResponseExecutionPath responsePath) {
        final List<ZscriptResponse> actualResponses = new ArrayList<>();

        final Iterator<NotificationSection<?>> sectionIt = getSections().iterator();
        final Iterator<Response>               respIt    = responsePath.iterator();

        for (int i = 0; sectionIt.hasNext() && respIt.hasNext(); i++) {
            final NotificationSection<?> section  = sectionIt.next();
            final Response               response = respIt.next();
            final ZscriptResponse        content  = section.parseResponse(response.getFields());
            if (!content.isValid()) {
                throw new ZscriptClientException("Invalid notification section [ntf=%s, section#=%s, section=%s, text=%s]", this, i, section, response);
            }

            actualResponses.add(content);
        }
        if (sectionIt.hasNext()) {
            throw new IllegalStateException("Notification needs more response sections");
        }
        if (respIt.hasNext()) {
            throw new IllegalStateException("Too many response sections received for notification");
        }
        return actualResponses;
    }
}
