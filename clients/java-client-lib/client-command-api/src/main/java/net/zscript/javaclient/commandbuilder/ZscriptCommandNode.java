package net.zscript.javaclient.commandbuilder;

import java.util.ArrayList;
import java.util.List;

import net.zscript.javareceiver.tokenizer.ZscriptExpression;
import net.zscript.model.components.Zchars;

public abstract class ZscriptCommandNode extends CommandSequenceNode {
    /**
     * Represents the next command from a given point in the Syntax Tree based on the AND/OR logic of a command sequence.
     */
    public static class ZscriptSequenceLogicPath {
        private final ZscriptCommandNode next;
        private final List<Byte>         markers;

        public ZscriptSequenceLogicPath(List<Byte> markers, ZscriptCommandNode next) {
            this.markers = markers;
            this.next = next;
        }

        public ZscriptSequenceLogicPath(List<Byte> markers, ZscriptSequenceLogicPath rest) {
            this.markers = new ArrayList<>(markers);
            this.markers.addAll(rest.markers);
            this.next = rest.next;
        }

        public List<Byte> getMarkers() {
            return markers;
        }

        public ZscriptCommandNode getNext() {
            return next;
        }
    }

    public abstract void onNotExecuted();

    /**
     * Performs a command's listener callbacks when given a command's response.
     *
     * @param resp
     */
    public abstract void onResponse(ZscriptExpression resp);

    public static ZscriptSequenceLogicPath findFirstCommand(final CommandSequenceNode start) {
        CommandSequenceNode current = start;
        CommandSequenceNode prev;
        List<Byte>          markers = new ArrayList<>();
        while (!current.isCommand()) {
            if (current.getClass() == AndSequenceNode.class) {
                prev = current;
                current = ((AndSequenceNode) current).getElements().get(0);
            } else if (current.getClass() == OrSequenceNode.class) {
                prev = current;
                current = ((OrSequenceNode) current).before;
            } else {
                throw new IllegalStateException("Unknown CommandSeqElement: " + current);
            }

            if (current.getClass() == OrSequenceNode.class && prev.getClass() == OrSequenceNode.class) {
                markers.add(Zchars.Z_OPENPAREN);
            }
        }
        return new ZscriptSequenceLogicPath(markers, (ZscriptCommandNode) current);
    }

    public ZscriptSequenceLogicPath findNext() {
        CommandSequenceNode current = parent;
        CommandSequenceNode prev    = this;
        List<Byte>          markers = new ArrayList<>();

        while (current != null) {
            if (current.getClass() == OrSequenceNode.class) {
                OrSequenceNode orAncestor = (OrSequenceNode) current;
                if (prev == orAncestor.before) {
                    markers.add(Zchars.Z_ORELSE);
                    return new ZscriptSequenceLogicPath(markers, findFirstCommand(orAncestor.after));
                }
            } else if (current.getClass() == AndSequenceNode.class) {
                AndSequenceNode andAncestor = (AndSequenceNode) current;
                if (prev != andAncestor.getElements().get(andAncestor.getElements().size() - 1)) {
                    CommandSequenceNode next = andAncestor.getElements().get(andAncestor.getElements().indexOf(prev) + 1);
                    if (prev.getClass() != OrSequenceNode.class && next.getClass() != OrSequenceNode.class) {
                        markers.add(Zchars.Z_ANDTHEN);
                    }
                    return new ZscriptSequenceLogicPath(markers, findFirstCommand(next));
                }
            }

            if (current.getClass() == OrSequenceNode.class && current.getParent() != null && current.getParent().getClass() != OrSequenceNode.class) {
                markers.add(Zchars.Z_CLOSEPAREN);
            }
            prev = current;
            current = current.getParent();
        }
        return null;
    }

    public ZscriptSequenceLogicPath findFailPath() {
        CommandSequenceNode current = parent;
        CommandSequenceNode prev    = this;
        List<Byte>          markers = new ArrayList<>();

        while (current != null) {
            if (current.getClass() == OrSequenceNode.class) {
                OrSequenceNode orAncestor = (OrSequenceNode) current;
                if (prev == orAncestor.before) {
                    markers.add(Zchars.Z_ORELSE);
                    return new ZscriptSequenceLogicPath(markers, findFirstCommand(orAncestor.after));
                }
            }
            if (current.getClass() == OrSequenceNode.class && current.getParent() != null && current.getParent().getClass() != OrSequenceNode.class) {
                markers.add(Zchars.Z_CLOSEPAREN);
            }
            prev = current;
            current = current.getParent();
        }
        return null;
    }

    public ZscriptSequenceLogicPath findSuccessPath() {
        CommandSequenceNode current = parent;
        CommandSequenceNode prev    = this;
        List<Byte>          markers = new ArrayList<>();

        while (current != null) {
            if (current.getClass() == AndSequenceNode.class) {
                AndSequenceNode andAncestor = (AndSequenceNode) current;
                if (prev != andAncestor.getElements().get(andAncestor.getElements().size() - 1)) {
                    CommandSequenceNode next = andAncestor.getElements().get(andAncestor.getElements().indexOf(prev) + 1);
                    if (prev.getClass() != OrSequenceNode.class && next.getClass() != OrSequenceNode.class) {
                        markers.add(Zchars.Z_ANDTHEN);
                    }
                    return new ZscriptSequenceLogicPath(markers, findFirstCommand(next));
                }
            }
            if (current.getClass() == OrSequenceNode.class && current.getParent() != null && current.getParent().getClass() != OrSequenceNode.class) {
                markers.add(Zchars.Z_CLOSEPAREN);
            }
            prev = current;
            current = current.getParent();
        }
        return null;
    }
}
