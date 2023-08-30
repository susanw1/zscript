package net.zscript.javaclient.commandbuilder;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

public class AndSequence extends CommandSequence {
    private final List<CommandSequence> elements;

    AndSequence(List<CommandSequence> elements) {
        this.elements = elements;
        for (CommandSequence el : elements) {
            el.setParent(this);
        }
    }

    AndSequence(CommandSequence el1, CommandSequence el2) {
        this.elements = new ArrayList<>();
        elements.add(el1);
        elements.add(el2);
        el1.setParent(this);
        el2.setParent(this);
    }

    @Override
    public CommandSequence andThen(CommandSequence next) {
        List<CommandSequence> newEls = new ArrayList<>(elements);
        if (next.getClass() == AndSequence.class) {
            newEls.addAll(((AndSequence) next).elements);
        } else {
            newEls.add(next);
        }
        return new AndSequence(newEls);
    }

    @Override
    public boolean canFail() {
        for (CommandSequence element : elements) {
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
    public CommandSequence reEvaluate() {
        List<CommandSequence> els = new ArrayList<>();
        for (CommandSequence element : elements) {
            if (element.getClass() == FailureCommand.class) {
                els.add(element);
                break;
            }
            if (element.getClass() == AbortCommand.class) {
                els.add(element);
                break;
            } else if (element.getClass() == AndSequence.class) {
                els.addAll(((AndSequence) element).elements);
            } else if (element.getClass() == BlankCommand.class) {
            } else {
                els.add(element.reEvaluate());
            }
        }
        if (els.size() == 0) {
            return new BlankCommand();
        }
        if (els.size() == 1) {
            return els.get(0);
        }
        return new AndSequence(els);
    }

    @Override
    public byte[] compile(boolean includeParens) {
        ByteArrayOutputStream out = new ByteArrayOutputStream();

        boolean useAnd = false;
        for (Iterator<CommandSequence> iterator = elements.iterator(); iterator.hasNext(); ) {
            CommandSequence el = iterator.next();
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

    public List<CommandSequence> getElements() {
        return elements;
    }
}
