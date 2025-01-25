package net.zscript.javaclient.commandbuilderapi.nodes;

import java.util.List;

public abstract class AbstractLogicSequenceNode extends CommandSequenceNode {
    final List<CommandSequenceNode> elements;

    public AbstractLogicSequenceNode(List<CommandSequenceNode> elements) {
        if (elements.isEmpty()) {
            throw new IllegalArgumentException("sequence empty");
        }
        this.elements = elements;
        elements.forEach(e -> e.setParent(this));
    }

    @Override
    boolean isCommand() {
        return false;
    }

    public List<CommandSequenceNode> getChildren() {
        return elements;
    }
}
