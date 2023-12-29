package net.zscript.javaclient.devices;

import java.text.ParseException;
import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Optional;
import java.util.Set;
import java.util.stream.Collectors;

import net.zscript.javaclient.commandPaths.Command;
import net.zscript.javaclient.commandPaths.MatchedCommandResponse;
import net.zscript.javaclient.commandPaths.Response;
import net.zscript.javaclient.commandbuilder.ZscriptResponse;
import net.zscript.javaclient.commandbuilder.commandnodes.ResponseCaptor;
import net.zscript.javaclient.commandbuilder.commandnodes.ZscriptCommandNode;
import net.zscript.javaclient.commandbuilder.notifications.NotificationSection;
import net.zscript.model.components.Zchars;

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

    public static NotificationSequenceCallback from(List<NotificationSection<?>> sections, List<Response> responses) {
        LinkedHashMap<NotificationSection<?>, ZscriptResponse> map = new LinkedHashMap<>();

        Iterator<NotificationSection<?>> sectionIt = sections.iterator();
        Iterator<Response>               respIt    = responses.iterator();

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

    public List<ZscriptResponse> getResponses() {
        return new ArrayList<>(responses.values());
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
