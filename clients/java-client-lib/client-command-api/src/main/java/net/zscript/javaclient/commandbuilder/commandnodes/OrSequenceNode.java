package net.zscript.javaclient.commandbuilder.commandnodes;

import java.util.List;

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
