package net.zscript.javaclient.commandbuilder;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.UncheckedIOException;

public class OrSequenceNode extends CommandSequenceNode {
    final CommandSequenceNode before;
    final CommandSequenceNode after;

    public OrSequenceNode(CommandSequenceNode before, CommandSequenceNode after) {
        this.before = before;
        this.after = after;
        before.setParent(this);
        after.setParent(this);
    }

    @Override
    protected boolean canFail() {
        return after.canFail();
    }

    @Override
    boolean isCommand() {
        return false;
    }

    @Override
    CommandSequenceNode reEvaluate() {
        if (!before.canFail()) {
            return before.reEvaluate();
        }
        return new OrSequenceNode(before.reEvaluate(), after.reEvaluate());
    }

    @Override
    public byte[] compile(boolean includeParens) {
        ByteArrayOutputStream out = new ByteArrayOutputStream();
        if (includeParens) {
            out.write((byte) '(');
        }
        try {
            out.write(before.compile(false));
            out.write((byte) '|');
            out.write(after.compile(false));
        } catch (IOException e) {
            throw new UncheckedIOException(e);
        }
        if (includeParens) {
            out.write((byte) ')');
        }
        return out.toByteArray();
    }
}
