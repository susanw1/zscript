package net.zscript.javaclient.commandbuilderapi.nodes;

import java.util.ArrayList;
import java.util.List;

import static java.util.Arrays.asList;

import net.zscript.javaclient.commandbuilderapi.defaultcommands.BlankCommandNode;
import net.zscript.model.components.Zchars;
import net.zscript.util.ByteString;

public final class OrSequenceNode extends AbstractLogicSequenceNode {
    OrSequenceNode(CommandSequenceNode... elements) {
        this(asList(elements));
    }

    OrSequenceNode(List<CommandSequenceNode> elements) {
        super(elements);
    }

    @Override
    protected boolean canFail() {
        return elements.get(elements.size() - 1).canFail();
    }

    @Override
    public CommandSequenceNode onFail(CommandSequenceNode next) {
        List<CommandSequenceNode> newEls = new ArrayList<>(elements);
        if (next.getClass() == OrSequenceNode.class) {
            newEls.addAll(((OrSequenceNode) next).elements);
        } else {
            newEls.add(next);
        }
        return new OrSequenceNode(newEls);
    }

    @Override
    CommandSequenceNode optimize() {
        final List<CommandSequenceNode> newEls = new ArrayList<>();
        for (final CommandSequenceNode e : elements) {
            final CommandSequenceNode optimized = e.optimize();
            if (optimized.getClass() == OrSequenceNode.class) {
                newEls.addAll(((OrSequenceNode) optimized).elements);
            } else {
                newEls.add(optimized);
            }
            if (!e.canFail()) {
                break;
            }
        }
        if (newEls.isEmpty()) { // is this even possible?
            return new BlankCommandNode();
        }
        if (newEls.size() == 1) {
            return newEls.get(0);
        }
        return new OrSequenceNode(newEls);
    }

    @Override
    public void appendTo(ByteString.ByteStringBuilder builder) {
        final boolean p = getParent() != null;
        if (p) {
            builder.appendByte(Zchars.Z_OPENPAREN);
        }
        builder.appendJoining(elements, b -> b.appendByte(Zchars.Z_ORELSE));
        if (p) {
            builder.appendByte(Zchars.Z_CLOSEPAREN);
        }
    }
}
