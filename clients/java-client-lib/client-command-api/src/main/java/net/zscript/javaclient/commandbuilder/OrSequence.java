package net.zscript.javaclient.commandbuilder;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.UncheckedIOException;

public class OrSequence extends CommandSequence {
    final CommandSequence before;
    final CommandSequence after;

    public OrSequence(CommandSequence before, CommandSequence after) {
        this.before = before;
        this.after = after;
        before.setParent(this);
        after.setParent(this);
    }

    @Override
    public boolean canFail() {
        return after.canFail();
    }

    @Override
    public boolean isCommand() {
        return false;
    }

    @Override
    public CommandSequence reEvaluate() {
        if (!before.canFail()) {
            return before.reEvaluate();
        }
        return new OrSequence(before.reEvaluate(), after.reEvaluate());
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
