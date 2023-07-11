package org.zcode.javaclient.zcodeApi;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

public class ZcodeAndSeqElement extends CommandSeqElement {
    private final List<CommandSeqElement> elements;

    ZcodeAndSeqElement(List<CommandSeqElement> elements) {
        this.elements = elements;
        for (CommandSeqElement el : elements) {
            el.setParent(this);
        }
    }

    ZcodeAndSeqElement(CommandSeqElement el1, CommandSeqElement el2) {
        this.elements = new ArrayList<>();
        elements.add(el1);
        elements.add(el2);
        el1.setParent(this);
        el2.setParent(this);
    }

    @Override
    public CommandSeqElement andThen(CommandSeqElement next) {
        List<CommandSeqElement> newEls = new ArrayList<>(elements);
        if (next.getClass() == ZcodeAndSeqElement.class) {
            newEls.addAll(((ZcodeAndSeqElement) next).elements);
        } else {
            newEls.add(next);
        }
        return new ZcodeAndSeqElement(newEls);
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
            if (element.getClass() == ZcodeFailureCommand.class) {
                els.add(element);
                break;
            } else if (element.getClass() == ZcodeAbortCommand.class) {
                els.add(element);
                break;
            } else if (element.getClass() == ZcodeAndSeqElement.class) {
                els.addAll(((ZcodeAndSeqElement) element).elements);
            } else if (element.getClass() == ZcodeBlankCommand.class) {
            } else {
                els.add(element.reEvaluate());
            }
        }
        if (els.size() == 0) {
            return new ZcodeBlankCommand();
        } else if (els.size() == 1) {
            return els.get(0);
        }
        return new ZcodeAndSeqElement(els);
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
