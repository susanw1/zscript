package net.zscript.javaclient.zscriptapi;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

public class AndSeqElement extends CommandSeqElement {
    private final List<CommandSeqElement> elements;

    AndSeqElement(List<CommandSeqElement> elements) {
        this.elements = elements;
        for (CommandSeqElement el : elements) {
            el.setParent(this);
        }
    }

    AndSeqElement(CommandSeqElement el1, CommandSeqElement el2) {
        this.elements = new ArrayList<>();
        elements.add(el1);
        elements.add(el2);
        el1.setParent(this);
        el2.setParent(this);
    }

    @Override
    public CommandSeqElement andThen(CommandSeqElement next) {
        List<CommandSeqElement> newEls = new ArrayList<>(elements);
        if (next.getClass() == AndSeqElement.class) {
            newEls.addAll(((AndSeqElement) next).elements);
        } else {
            newEls.add(next);
        }
        return new AndSeqElement(newEls);
    }

    @Override
    public boolean canFail() {
        for (CommandSeqElement element : elements) {
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
    public CommandSeqElement reEvaluate() {
        List<CommandSeqElement> els = new ArrayList<>();
        for (CommandSeqElement element : elements) {
            if (element.getClass() == FailureCommand.class) {
                els.add(element);
                break;
            } else if (element.getClass() == AbortCommand.class) {
                els.add(element);
                break;
            } else if (element.getClass() == AndSeqElement.class) {
                els.addAll(((AndSeqElement) element).elements);
            } else if (element.getClass() == BlankCommand.class) {
            } else {
                els.add(element.reEvaluate());
            }
        }
        if (els.size() == 0) {
            return new BlankCommand();
        } else if (els.size() == 1) {
            return els.get(0);
        }
        return new AndSeqElement(els);
    }

    @Override
    public byte[] compile(boolean includeParens) {
        ByteArrayOutputStream out = new ByteArrayOutputStream();

        boolean useAnd = false;
        for (Iterator<CommandSeqElement> iterator = elements.iterator(); iterator.hasNext();) {
            CommandSeqElement el = iterator.next();
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

    public List<CommandSeqElement> getElements() {
        return elements;
    }
}
