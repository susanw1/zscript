package net.zscript.javaclient.commandbuilderapi.nodes;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import net.zscript.javaclient.commandbuilderapi.defaultcommands.AbortCommandNode;
import net.zscript.javaclient.commandbuilderapi.defaultcommands.BlankCommandNode;
import net.zscript.javaclient.commandbuilderapi.defaultcommands.FailureCommandNode;
import net.zscript.model.components.Zchars;
import net.zscript.util.ByteString.ByteStringBuilder;

public final class AndSequenceNode extends AbstractLogicSequenceNode {
    AndSequenceNode(List<CommandSequenceNode> elements) {
        super(elements);
    }

    AndSequenceNode(CommandSequenceNode... el) {
        this(Arrays.asList(el));
    }

    @Override
    public CommandSequenceNode andThen(CommandSequenceNode next) {
        List<CommandSequenceNode> newEls = new ArrayList<>(elements);
        if (next.getClass() == AndSequenceNode.class) {
            newEls.addAll(((AndSequenceNode) next).elements);
        } else {
            newEls.add(next);
        }
        return new AndSequenceNode(newEls);
    }

    @Override
    public boolean canFail() {
        return elements.stream()
                .anyMatch(CommandSequenceNode::canFail);
    }

    @Override
    CommandSequenceNode optimize() {
        final List<CommandSequenceNode> els = new ArrayList<>();
        for (final CommandSequenceNode element : elements) {
            if (element.getClass() == FailureCommandNode.class || element.getClass() == AbortCommandNode.class) {
                els.add(element);
                break;
            } else if (element.getClass() == AndSequenceNode.class) {
                els.addAll(((AndSequenceNode) element).elements);

            } else if (element.getClass() == BlankCommandNode.class || element instanceof ZscriptCommandNode && ((ZscriptCommandNode<?>) element).asFieldSet().isEmpty()) {
            } else {
                els.add(element.optimize());
            }
        }
        if (els.isEmpty()) {
            return new BlankCommandNode();
        }
        if (els.size() == 1) {
            return els.get(0);
        }
        return new AndSequenceNode(els);
    }

    //|| element.getClass() == ZscriptCommandNode.class
    @Override
    public void appendTo(ByteStringBuilder builder) {
        builder.appendJoining(elements, b -> b.appendByte(Zchars.Z_ANDTHEN));
    }
}
