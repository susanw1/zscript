package net.zscript.javaclient.devices;

import java.util.ArrayList;
import java.util.Collection;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Optional;
import java.util.Set;
import java.util.stream.Collectors;

import net.zscript.javaclient.commandPaths.Response;
import net.zscript.javaclient.commandbuilder.ZscriptResponse;
import net.zscript.javaclient.commandbuilder.commandnodes.ResponseCaptor;
import net.zscript.javaclient.commandbuilder.commandnodes.ZscriptCommandNode;
import net.zscript.javaclient.commandbuilder.notifications.NotificationSection;

public class NotificationSequenceCallback {

    public static class NotificationSectionSummary<T extends ZscriptResponse> {
        private final NotificationSection<T> notif;
        private final T                      response;

        public static <T extends ZscriptResponse> NotificationSectionSummary<T> generateExecutionSummary(NotificationSection<T> notif, Response response) {
            return new NotificationSectionSummary<>(notif, notif.parseResponse(response.getFields()));
        }

        public NotificationSectionSummary(NotificationSection<T> notif, T response) {
            this.notif = notif;
            this.response = response;
        }

        public NotificationSection<T> getNotification() {
            return notif;
        }

        public T getResponse() {
            return response;
        }

        public Class<T> getResponseType() {
            return notif.getResponseType();
        }
    }

    private final LinkedHashMap<NotificationSection<?>, ZscriptResponse> responses;

    private final Set<NotificationSection<?>>        notExecuted;
    private final Set<NotificationSectionSummary<?>> succeeded;
    private final Set<NotificationSectionSummary<?>> failed;

    private final NotificationSectionSummary<?> abort;

    private NotificationSequenceCallback(LinkedHashMap<NotificationSection<?>, ZscriptResponse> responses, Set<NotificationSection<?>> notExecuted,
            Set<NotificationSectionSummary<?>> succeeded,
            Set<NotificationSectionSummary<?>> failed, NotificationSectionSummary<?> abort) {
        this.responses = responses;
        this.notExecuted = notExecuted;
        this.succeeded = succeeded;
        this.failed = failed;
        this.abort = abort;
    }

    public List<ZscriptResponse> getResponses() {
        return new ArrayList<>(responses.values());
    }

    public Collection<NotificationSection<?>> getNotExecuted() {
        return notExecuted;
    }

    public Collection<NotificationSectionSummary<?>> getSucceeded() {
        return succeeded;
    }

    public Collection<NotificationSectionSummary<?>> getFailed() {
        return failed;
    }

    public Optional<NotificationSectionSummary<?>> getAborted() {
        return Optional.ofNullable(abort);
    }

    public List<NotificationSection<?>> getExecuted() {
        return new ArrayList<>(responses.keySet());
    }

    public List<NotificationSectionSummary<?>> getExecutionSummary() {
        return responses.entrySet().stream().map(e -> matchExecutionSummary(e.getKey(), e.getValue())).collect(Collectors.toList());
    }

    private <T extends ZscriptResponse> NotificationSectionSummary<T> matchExecutionSummary(NotificationSection<T> node, ZscriptResponse resp) {
        return new NotificationSectionSummary<>(node, node.getResponseType().cast(resp));
    }

    public Optional<ZscriptResponse> getResponseFor(NotificationSection<?> node) {
        return Optional.ofNullable(responses.get(node));
    }

    public <T extends ZscriptResponse> Optional<T> getResponseFor(ResponseCaptor<T> captor) {
        return getResponseFor((NotificationSection<T>) captor.getSource()).map(r -> ((NotificationSection<T>) captor.getSource()).getResponseType().cast(r));
    }
}
