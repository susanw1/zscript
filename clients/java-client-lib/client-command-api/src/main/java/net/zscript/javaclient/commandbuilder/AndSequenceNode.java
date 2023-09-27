package net.zscript.javaclient.commandbuilder;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

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
    CommandSequenceNode reEvaluate() {
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
                els.add(element.reEvaluate());
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
        for (Iterator<CommandSequenceNode> iterator = elements.iterator(); iterator.hasNext(); ) {
            CommandSequenceNode el = iterator.next();
            try {
                byte[] data = el.compile(true);
                if (useAnd && data.length > 0 && data[0] != '(') {
                    out.write((byte) '&');
                }
                out.write(data);
                if (data.length == 0 || data[data.length - 1] != ')') {
                    useAnd = true;
                }
            } catch (IOException e) {
                throw new RuntimeException(e);
            }
        }
        return out.toByteArray();
    }

    public List<CommandSequenceNode> getElements() {
        return elements;
    }
}
