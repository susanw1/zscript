package net.zscript.javaclient.commandbuilder;

import java.io.ByteArrayOutputStream;
import java.io.IOException;

import net.zscript.model.components.Zchars;

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
    CommandSequenceNode optimize() {
        if (!before.canFail()) {
            return before.optimize();
        }
        return new OrSequenceNode(before.optimize(), after.optimize());
    }

    @Override
    public byte[] compile(boolean includeParens) throws IOException {
        ByteArrayOutputStream out = new ByteArrayOutputStream();
        if (includeParens) {
            out.write(Zchars.Z_OPENPAREN);
        }
        out.write(before.compile(false));
        out.write(Zchars.Z_ORELSE);
        out.write(after.compile(false));
        if (includeParens) {
            out.write(Zchars.Z_CLOSEPAREN);
        }
        return out.toByteArray();
    }
}
