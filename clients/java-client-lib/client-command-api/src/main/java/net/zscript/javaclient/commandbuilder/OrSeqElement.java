package net.zscript.javaclient.commandbuilder;

import java.io.ByteArrayOutputStream;
import java.io.IOException;

public class OrSeqElement extends CommandSeqElement {
    final CommandSeqElement before;
    final CommandSeqElement after;

    public OrSeqElement(CommandSeqElement before, CommandSeqElement after) {
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
    public CommandSeqElement reEvaluate() {
        if (!before.canFail()) {
            return before.reEvaluate();
        }
        return new OrSeqElement(before.reEvaluate(), after.reEvaluate());
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
            throw new RuntimeException(e);
        }
        if (includeParens) {
            out.write((byte) ')');
        }
        return out.toByteArray();
    }
}