package net.zscript.javaclient.devices;

import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Optional;
import java.util.Set;
import java.util.stream.Collectors;

import net.zscript.javaclient.commandPaths.Command;
import net.zscript.javaclient.commandPaths.MatchedCommandResponse;
import net.zscript.javaclient.commandbuilder.ZscriptCommandNode;
import net.zscript.javaclient.commandbuilder.ZscriptResponse;
import net.zscript.model.components.Zchars;

public class ResponseSequenceCallback {

    public static class CommandExecutionSummary {
        private final ZscriptCommandNode command;
        private final ZscriptResponse    response;

        public CommandExecutionSummary(ZscriptCommandNode command, ZscriptResponse response) {
            this.command = command;
            this.response = response;
        }

        public ZscriptCommandNode getCommand() {
            return command;
        }

        public ZscriptResponse getResponse() {
            return response;
        }
    }

    public static ResponseSequenceCallback from(List<MatchedCommandResponse> matchedCRs, Iterable<ZscriptCommandNode> nodes, Map<ZscriptCommandNode, Command> commandMap) {
        Map<Command, ZscriptCommandNode> nodeMap = new HashMap<>();
        for (Map.Entry<ZscriptCommandNode, Command> e : commandMap.entrySet()) {
            nodeMap.put(e.getValue(), e.getKey());
        }
        Set<ZscriptCommandNode> notExecuted = new HashSet<>();
        for (ZscriptCommandNode node : nodes) {
            if (!notExecuted.add(node)) {
                throw new IllegalStateException(
                        "Command tree contains duplicate ZscriptCommandNode - this is not supported. Instead share the builder, and call it twice, or create the commands seperately.");
            }
        }

        LinkedHashMap<ZscriptCommandNode, ZscriptResponse> responses = new LinkedHashMap<>();

        Set<CommandExecutionSummary> succeeded = new HashSet<>();
        Set<CommandExecutionSummary> failed    = new HashSet<>();
        CommandExecutionSummary      abort     = null;

        for (MatchedCommandResponse cr : matchedCRs) {
            ZscriptCommandNode      node    = nodeMap.get(cr.getCommand());
            ZscriptResponse         resp    = node.onResponse(cr.getResponse().getFields());
            CommandExecutionSummary summary = new CommandExecutionSummary(node, resp);
            responses.put(node, resp);
            if (cr.getResponse().wasSuccess()) {
                succeeded.add(summary);
            } else if (cr.getResponse().getFields().getField(Zchars.Z_STATUS).orElse(0) < 0x10) {
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

    private final LinkedHashMap<ZscriptCommandNode, ZscriptResponse> responses;

    private final Set<ZscriptCommandNode>      notExecuted;
    private final Set<CommandExecutionSummary> succeeded;
    private final Set<CommandExecutionSummary> failed;

    private final CommandExecutionSummary abort;

    private ResponseSequenceCallback(LinkedHashMap<ZscriptCommandNode, ZscriptResponse> responses, Set<ZscriptCommandNode> notExecuted, Set<CommandExecutionSummary> succeeded,
            Set<CommandExecutionSummary> failed, CommandExecutionSummary abort) {
        this.responses = responses;
        this.notExecuted = notExecuted;
        this.succeeded = succeeded;
        this.failed = failed;
        this.abort = abort;
    }

    public List<ZscriptResponse> getResponses() {
        return new ArrayList<>(responses.values());
    }

    public Collection<ZscriptCommandNode> getNotExecuted() {
        return notExecuted;
    }

    public Collection<CommandExecutionSummary> getSucceeded() {
        return succeeded;
    }

    public Collection<CommandExecutionSummary> getFailed() {
        return failed;
    }

    public Optional<CommandExecutionSummary> getAborted() {
        return Optional.ofNullable(abort);
    }

    public List<ZscriptCommandNode> getExecuted() {
        return new ArrayList<>(responses.keySet());
    }

    public List<CommandExecutionSummary> getExecutionSummary() {
        return responses.entrySet().stream().map(e -> new CommandExecutionSummary(e.getKey(), e.getValue())).collect(Collectors.toList());
    }

    public Optional<ZscriptResponse> getResponseFor(ZscriptCommandNode node) {
        return Optional.ofNullable(responses.get(node));
    }
}
