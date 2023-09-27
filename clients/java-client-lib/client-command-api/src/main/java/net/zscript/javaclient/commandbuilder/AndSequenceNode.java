package net.zscript.javaclient.commandbuilder;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.UncheckedIOException;
import java.util.ArrayList;
import java.util.List;

import net.zscript.model.components.Zchars;

public class AndSequenceNode extends CommandSequenceNode {
    private final List<CommandSequenceNode> elements;

    AndSequenceNode(List<CommandSequenceNode> elements) {
        this.elements = elements;
        for (CommandSequenceNode el : elements) {
            el.setParent(this);
        }
    }

    AndSequenceNode(CommandSequenceNode el1, CommandSequenceNode el2) {
        this.elements = new ArrayList<>();
        elements.add(el1);
        elements.add(el2);
        el1.setParent(this);
        el2.setParent(this);
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
        for (CommandSequenceNode element : elements) {
            if (element.canFail()) {
                return true;
            }
        }
        return false;
    }

    @Override
    public boolean isCommand() {
        return false;
    }

    @Override
    CommandSequenceNode optimize() {
        List<CommandSequenceNode> els = new ArrayList<>();
        for (CommandSequenceNode element : elements) {
            if (element.getClass() == FailureCommandNode.class) {
                els.add(element);
                break;
            }
            if (element.getClass() == AbortCommandNode.class) {
                els.add(element);
                break;
            } else if (element.getClass() == AndSequenceNode.class) {
                els.addAll(((AndSequenceNode) element).elements);
            } else if (element.getClass() == BlankCommandNode.class) {
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

    @Override
    byte[] compile(boolean includeParens) {
        ByteArrayOutputStream out = new ByteArrayOutputStream();

        boolean useAnd = false;
        try {
            for (CommandSequenceNode el : elements) {
                byte[] data = el.compile(true);
                if (useAnd && data.length > 0 && data[0] != Zchars.Z_OPENPAREN) {
                    out.write(Zchars.Z_ANDTHEN);
                }
                out.write(data);
                if (data.length == 0 || data[data.length - 1] != Zchars.Z_CLOSEPAREN) {
                    useAnd = true;
                }
            }
        } catch (IOException e) {
            throw new UncheckedIOException(e);
        }
        return out.toByteArray();
    }

    public List<CommandSequenceNode> getElements() {
        return elements;
    }
}
