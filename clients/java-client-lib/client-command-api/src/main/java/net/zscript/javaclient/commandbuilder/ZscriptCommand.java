package net.zscript.javaclient.commandbuilder;

import java.util.ArrayList;
import java.util.List;

import net.zscript.javareceiver.tokenizer.ZscriptExpression;
import net.zscript.model.components.Zchars;

public abstract class ZscriptCommand extends CommandSequence {
    /**
     * Represents the next command from a given point in the Syntax Tree based on the AND/OR logic of a command sequence.
     */
    public static class ZscriptSequencePath {
        private final List<Byte>     markers;
        private final ZscriptCommand next;

        public ZscriptSequencePath(List<Byte> markers, ZscriptCommand next) {
            this.markers = markers;
            this.next = next;
        }

        public ZscriptSequencePath(List<Byte> markers, ZscriptSequencePath rest) {
            this.markers = new ArrayList<>(markers);
            this.markers.addAll(rest.markers);
            this.next = rest.next;
        }

        public List<Byte> getMarkers() {
            return markers;
        }

        public ZscriptCommand getNext() {
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

    public static ZscriptSequencePath findFirstCommand(final CommandSequence start) {
        CommandSequence current = start;
        CommandSequence prev;
        List<Byte>      markers = new ArrayList<>();
        while (!current.isCommand()) {
            if (current.getClass() == AndSequence.class) {
                prev = current;
                current = ((AndSequence) current).getElements().get(0);
            } else if (current.getClass() == OrSequence.class) {
                prev = current;
                current = ((OrSequence) current).before;
            } else {
                throw new IllegalStateException("Unknown CommandSeqElement: " + current);
            }

            if (current.getClass() == OrSequence.class && prev.getClass() == OrSequence.class) {
                markers.add(Zchars.Z_OPENPAREN);
            }
        }
        return new ZscriptSequencePath(markers, (ZscriptCommand) current);
    }

    public ZscriptSequencePath findNext() {
        CommandSequence current = parent;
        CommandSequence prev    = this;
        List<Byte>      markers = new ArrayList<>();

        while (current != null) {
            if (current.getClass() == OrSequence.class) {
                OrSequence orAncestor = (OrSequence) current;
                if (prev == orAncestor.before) {
                    markers.add(Zchars.Z_ORELSE);
                    return new ZscriptSequencePath(markers, findFirstCommand(orAncestor.after));
                }
            } else if (current.getClass() == AndSequence.class) {
                AndSequence andAncestor = (AndSequence) current;
                if (prev != andAncestor.getElements().get(andAncestor.getElements().size() - 1)) {
                    CommandSequence next = andAncestor.getElements().get(andAncestor.getElements().indexOf(prev) + 1);
                    if (prev.getClass() != OrSequence.class && next.getClass() != OrSequence.class) {
                        markers.add(Zchars.Z_ANDTHEN);
                    }
                    return new ZscriptSequencePath(markers, findFirstCommand(next));
                }
            }

            if (current.getClass() == OrSequence.class && current.getParent() != null && current.getParent().getClass() != OrSequence.class) {
                markers.add(Zchars.Z_CLOSEPAREN);
            }
            prev = current;
            current = current.getParent();
        }
        return null;
    }

    public ZscriptSequencePath findFailPath() {
        CommandSequence current = parent;
        CommandSequence prev    = this;
        List<Byte>      markers = new ArrayList<>();

        while (current != null) {
            if (current.getClass() == OrSequence.class) {
                OrSequence orAncestor = (OrSequence) current;
                if (prev == orAncestor.before) {
                    markers.add(Zchars.Z_ORELSE);
                    return new ZscriptSequencePath(markers, findFirstCommand(orAncestor.after));
                }
            }
            if (current.getClass() == OrSequence.class && current.getParent() != null && current.getParent().getClass() != OrSequence.class) {
                markers.add(Zchars.Z_CLOSEPAREN);
            }
            prev = current;
            current = current.getParent();
        }
        return null;
    }

    public ZscriptSequencePath findSuccessPath() {
        CommandSequence current = parent;
        CommandSequence prev    = this;
        List<Byte>      markers = new ArrayList<>();

        while (current != null) {
            if (current.getClass() == AndSequence.class) {
                AndSequence andAncestor = (AndSequence) current;
                if (prev != andAncestor.getElements().get(andAncestor.getElements().size() - 1)) {
                    CommandSequence next = andAncestor.getElements().get(andAncestor.getElements().indexOf(prev) + 1);
                    if (prev.getClass() != OrSequence.class && next.getClass() != OrSequence.class) {
                        markers.add(Zchars.Z_ANDTHEN);
                    }
                    return new ZscriptSequencePath(markers, findFirstCommand(next));
                }
            }
            if (current.getClass() == OrSequence.class && current.getParent() != null && current.getParent().getClass() != OrSequence.class) {
                markers.add(Zchars.Z_CLOSEPAREN);
            }
            prev = current;
            current = current.getParent();
        }
        return null;
    }
}
