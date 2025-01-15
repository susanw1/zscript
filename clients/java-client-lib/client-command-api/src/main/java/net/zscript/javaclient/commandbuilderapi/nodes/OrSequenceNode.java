package net.zscript.javaclient.commandbuilderapi.nodes;

import java.util.List;

import net.zscript.model.components.Zchars;
import net.zscript.util.ByteString;
import net.zscript.util.ByteString.ByteAppendable;

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

    public List<CommandSequenceNode> getChildren() {
        return List.of(before, after);
    }

    @Override
    public void appendTo(ByteString.ByteStringBuilder builder) {
        builder.appendByte(Zchars.Z_OPENPAREN)
                .append((ByteAppendable) before)
                .appendByte(Zchars.Z_ORELSE)
                .append((ByteAppendable) after)
                .appendByte(Zchars.Z_CLOSEPAREN);
    }
}
