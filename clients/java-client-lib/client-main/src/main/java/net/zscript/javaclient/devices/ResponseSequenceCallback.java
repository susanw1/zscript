package net.zscript.javaclient.devices;

import javax.annotation.Nonnull;
import javax.annotation.Nullable;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Optional;
import java.util.Set;
import java.util.stream.Collectors;

import net.zscript.javaclient.commandbuilderapi.Respondable;
import net.zscript.javaclient.commandbuilderapi.ZscriptResponse;
import net.zscript.javaclient.commandbuilderapi.nodes.ResponseCaptor;
import net.zscript.javaclient.commandbuilderapi.nodes.ZscriptCommandNode;
import net.zscript.javaclient.commandpaths.CommandElement;
import net.zscript.javaclient.commandpaths.MatchedCommandResponse;
import net.zscript.javaclient.commandpaths.ResponseElement;
import net.zscript.model.components.Zchars;
import net.zscript.model.components.ZscriptStatus;

public class ResponseSequenceCallback {

    @Nonnull
    public static ResponseSequenceCallback from(List<MatchedCommandResponse> matchedCRs, Iterable<ZscriptCommandNode<?>> nodes,
            Map<ZscriptCommandNode<?>, CommandElement> commandMap) {
        Set<ZscriptCommandNode<?>> notExecuted = new HashSet<>();
        for (ZscriptCommandNode<?> node : nodes) {
            if (!notExecuted.add(node)) {
                throw new IllegalStateException(
                        "Command tree contains duplicate ZscriptCommandNode - this is not supported. Instead share the builder, and call it twice, or create the commands separately.");
            }
        }
        if (matchedCRs.isEmpty()) {
            // if nothing was executed:
            return new ResponseSequenceCallback(notExecuted);
        }
        Map<CommandElement, ZscriptCommandNode<?>> nodeMap = new HashMap<>();
        for (Map.Entry<ZscriptCommandNode<?>, CommandElement> e : commandMap.entrySet()) {
            nodeMap.put(e.getValue(), e.getKey());
        }

        LinkedHashMap<ZscriptCommandNode<?>, ZscriptResponse> responses = new LinkedHashMap<>();

        Set<CommandExecutionSummary<?>> succeeded = new HashSet<>();
        Set<CommandExecutionSummary<?>> failed    = new HashSet<>();
        CommandExecutionSummary<?>      abort     = null;

        for (MatchedCommandResponse cr : matchedCRs) {
            ZscriptCommandNode<?>      node    = nodeMap.get(cr.getCommand());
            CommandExecutionSummary<?> summary = CommandExecutionSummary.generateExecutionSummary(node, cr.getResponse());
            responses.put(node, summary.getResponse());
            if (cr.getResponse().wasSuccess()) {
                succeeded.add(summary);
            } else if (ZscriptStatus.isFailure(cr.getResponse().getFields().getFieldValueOrDefault(Zchars.Z_STATUS, 0))) {
                failed.add(summary);
            } else {
                if (abort != null) {
                    throw new IllegalStateException("Response sequence has more than one error status - this should be impossible");
                }
                abort = summary;
            }
            notExecuted.remove(node);
        }
        return new ResponseSequenceCallback(responses, notExecuted, succeeded, failed, abort);
    }

    private final LinkedHashMap<ZscriptCommandNode<?>, ZscriptResponse> responses;

    private final Set<ZscriptCommandNode<?>>      notExecuted;
    private final Set<CommandExecutionSummary<?>> succeeded;
    private final Set<CommandExecutionSummary<?>> failed;

    private final CommandExecutionSummary<?> abort;

    private final boolean wasExecuted;

    private ResponseSequenceCallback(LinkedHashMap<ZscriptCommandNode<?>, ZscriptResponse> responses, Set<ZscriptCommandNode<?>> notExecuted,
            Set<CommandExecutionSummary<?>> succeeded,
            Set<CommandExecutionSummary<?>> failed, @Nullable CommandExecutionSummary<?> abort) {
        this.responses = responses;
        this.notExecuted = notExecuted;
        this.succeeded = succeeded;
        this.failed = failed;
        this.abort = abort;
        this.wasExecuted = true;
    }

    private ResponseSequenceCallback(Set<ZscriptCommandNode<?>> notExecuted) {
        this.responses = new LinkedHashMap<>();
        this.notExecuted = notExecuted;
        this.succeeded = Collections.emptySet();
        this.failed = Collections.emptySet();
        this.abort = null;
        this.wasExecuted = false;
    }

    public List<ZscriptResponse> getResponses() {
        return new ArrayList<>(responses.values());
    }

    public Collection<ZscriptCommandNode<?>> getNotExecuted() {
        return notExecuted;
    }

    public Collection<CommandExecutionSummary<?>> getSucceeded() {
        return succeeded;
    }

    public Collection<CommandExecutionSummary<?>> getFailed() {
        return failed;
    }

    public Optional<CommandExecutionSummary<?>> getAborted() {
        return Optional.ofNullable(abort);
    }

    public List<ZscriptCommandNode<?>> getExecuted() {
        return new ArrayList<>(responses.keySet());
    }

    public List<CommandExecutionSummary<?>> getExecutionSummary() {
        return responses.entrySet().stream().map(e -> matchExecutionSummary(e.getKey(), e.getValue())).collect(Collectors.toList());
    }

    private <T extends ZscriptResponse> CommandExecutionSummary<T> matchExecutionSummary(ZscriptCommandNode<T> node, ZscriptResponse resp) {
        return new CommandExecutionSummary<>(node, node.getResponseType().cast(resp));
    }

    public Optional<ZscriptResponse> getResponseFor(ZscriptCommandNode<?> node) {
        return Optional.ofNullable(responses.get(node));
    }

    public <T extends ZscriptResponse> Optional<T> getResponseFor(ResponseCaptor<T> captor) {
        // FIXME: ensure source can't be null!
        final Respondable<T> source = captor.getSource();
        return getResponseFor((ZscriptCommandNode<T>) source).map(r -> ((ZscriptCommandNode<T>) source).getResponseType().cast(r));
    }

    public boolean wasExecuted() {
        return wasExecuted;
    }

    /**
     * Pair wrapper that binds a ZscriptCommandNode and its response.
     *
     * @param <T> the type of response
     */
    public static class CommandExecutionSummary<T extends ZscriptResponse> {
        private final ZscriptCommandNode<T> command;
        private final T                     response;

        @Nonnull
        public static <T extends ZscriptResponse> CommandExecutionSummary<T> generateExecutionSummary(ZscriptCommandNode<T> command, ResponseElement response) {
            return new CommandExecutionSummary<>(command, command.parseResponse(response.getFields()));
        }

        public CommandExecutionSummary(ZscriptCommandNode<T> command, T response) {
            this.command = command;
            this.response = response;
        }

        @Nonnull
        public ZscriptCommandNode<T> getCommand() {
            return command;
        }

        @Nonnull
        public T getResponse() {
            return response;
        }

        @Nonnull
        public Class<T> getResponseType() {
            return command.getResponseType();
        }
    }
}
