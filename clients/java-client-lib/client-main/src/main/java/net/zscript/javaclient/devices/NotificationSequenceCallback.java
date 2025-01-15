package net.zscript.javaclient.devices;

import javax.annotation.Nonnull;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Optional;
import java.util.stream.Collectors;

import net.zscript.javaclient.commandbuilderapi.ZscriptResponse;
import net.zscript.javaclient.commandbuilderapi.nodes.ResponseCaptor;
import net.zscript.javaclient.commandbuilderapi.notifications.NotificationSection;
import net.zscript.javaclient.commandpaths.ResponseElement;

public class NotificationSequenceCallback {

    public static class NotificationSectionSummary<T extends ZscriptResponse> {
        private final NotificationSection<T> notif;
        private final T                      response;

        @Nonnull
        public static <T extends ZscriptResponse> NotificationSectionSummary<T> generateExecutionSummary(NotificationSection<T> notif, ResponseElement response) {
            return new NotificationSectionSummary<>(notif, notif.parseResponse(response.getFields()));
        }

        public NotificationSectionSummary(NotificationSection<T> notif, T response) {
            this.notif = notif;
            this.response = response;
        }

        @Nonnull
        public NotificationSection<T> getNotification() {
            return notif;
        }

        @Nonnull
        public T getResponse() {
            return response;
        }

        @Nonnull
        public Class<T> getResponseType() {
            return notif.getResponseType();
        }
    }

    @Nonnull
    public static NotificationSequenceCallback from(List<NotificationSection<?>> sections, List<ResponseElement> responses) {
        LinkedHashMap<NotificationSection<?>, ZscriptResponse> map = new LinkedHashMap<>();

        Iterator<NotificationSection<?>> sectionIt = sections.iterator();
        Iterator<ResponseElement>        respIt    = responses.iterator();

        while (sectionIt.hasNext() && respIt.hasNext()) {
            NotificationSection<?> section = sectionIt.next();
            map.put(section, section.parseResponse(respIt.next().getFields()));
        }
        if (respIt.hasNext()) {
            throw new IllegalStateException("Responses received longer than notification");
        }
        return new NotificationSequenceCallback(map);
    }

    private final LinkedHashMap<NotificationSection<?>, ZscriptResponse> responses;

    private NotificationSequenceCallback(LinkedHashMap<NotificationSection<?>, ZscriptResponse> responses) {
        this.responses = responses;
    }

    @Nonnull
    public List<ZscriptResponse> getResponses() {
        return new ArrayList<>(responses.values());
    }

    @Nonnull
    public List<NotificationSection<?>> getExecuted() {
        return new ArrayList<>(responses.keySet());
    }

    @Nonnull
    public List<NotificationSectionSummary<?>> getExecutionSummary() {
        return responses.entrySet().stream().map(e -> matchExecutionSummary(e.getKey(), e.getValue())).collect(Collectors.toList());
    }

    @Nonnull
    private <T extends ZscriptResponse> NotificationSectionSummary<T> matchExecutionSummary(NotificationSection<T> node, ZscriptResponse resp) {
        return new NotificationSectionSummary<>(node, node.getResponseType().cast(resp));
    }

    @Nonnull
    public Optional<ZscriptResponse> getResponseFor(NotificationSection<?> node) {
        return Optional.ofNullable(responses.get(node));
    }

    @Nonnull
    public <T extends ZscriptResponse> Optional<T> getResponseFor(ResponseCaptor<T> captor) {
        // FIXME: ensure source can't be null!
        return getResponseFor((NotificationSection<T>) captor.getSource()).map(r -> ((NotificationSection<T>) captor.getSource()).getResponseType().cast(r));
    }
}
