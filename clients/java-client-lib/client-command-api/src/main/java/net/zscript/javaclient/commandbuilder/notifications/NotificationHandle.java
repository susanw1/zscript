package net.zscript.javaclient.commandbuilder.notifications;

import javax.annotation.Nonnull;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import net.zscript.javaclient.ZscriptParseException;
import net.zscript.javaclient.commandbuilder.ZscriptResponse;
import net.zscript.javaclient.commandpaths.Response;
import net.zscript.javaclient.commandpaths.ResponseExecutionPath;

public abstract class NotificationHandle<N extends Notification> {
    @Nonnull
    public abstract <T extends ZscriptResponse> NotificationSection<T> getSection(Class<T> response);

    @Nonnull
    public abstract List<NotificationSection<?>> getSections();

    public abstract N createNotification(@Nonnull final ResponseExecutionPath responsePath);

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
                throw new ZscriptParseException("Invalid notification section [ntf=%s, section#=%s, section=%s, text=%s]", this, i, section, response);
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
