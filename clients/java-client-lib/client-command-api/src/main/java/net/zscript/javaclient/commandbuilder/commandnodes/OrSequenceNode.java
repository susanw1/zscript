package net.zscript.javaclient.commandbuilder.commandnodes;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.util.List;
import java.util.stream.Collectors;

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

    public List<CommandSequenceNode> getChildren() {
        return List.of(before, after);
    }

    public String asString() {
        return "(" + before.asString() + "|" + after.asString() + ")";
    }
}
