package net.zscript.javaclient.commandbuilder;

import java.util.ArrayList;
import java.util.List;

import net.zscript.javareceiver.tokenizer.ZscriptExpression;
import net.zscript.model.components.Zchars;

public abstract class ZscriptCommand extends CommandSeqElement {
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

    public static ZscriptSequencePath findFirstCommand(final CommandSeqElement start) {
        CommandSeqElement current = start;
        CommandSeqElement prev;
        List<Byte>        markers = new ArrayList<>();
        while (!current.isCommand()) {
            if (current.getClass() == AndSeqElement.class) {
                prev = current;
                current = ((AndSeqElement) current).getElements().get(0);
            } else if (current.getClass() == OrSeqElement.class) {
                prev = current;
                current = ((OrSeqElement) current).before;
            } else {
                throw new IllegalStateException("Unknown CommandSeqElement: " + current);
            }

            if (current.getClass() == OrSeqElement.class && prev.getClass() == OrSeqElement.class) {
                markers.add(Zchars.Z_OPENPAREN);
            }
        }
        return new ZscriptSequencePath(markers, (ZscriptCommand) current);
    }

    public ZscriptSequencePath findNext() {
        CommandSeqElement current = parent;
        CommandSeqElement prev    = this;
        List<Byte>        markers = new ArrayList<>();

        while (current != null) {
            if (current.getClass() == OrSeqElement.class) {
                OrSeqElement orAncestor = (OrSeqElement) current;
                if (prev == orAncestor.before) {
                    markers.add(Zchars.Z_ORELSE);
                    return new ZscriptSequencePath(markers, findFirstCommand(orAncestor.after));
                }
            } else if (current.getClass() == AndSeqElement.class) {
                AndSeqElement andAncestor = (AndSeqElement) current;
                if (prev != andAncestor.getElements().get(andAncestor.getElements().size() - 1)) {
                    CommandSeqElement next = andAncestor.getElements().get(andAncestor.getElements().indexOf(prev) + 1);
                    if (prev.getClass() != OrSeqElement.class && next.getClass() != OrSeqElement.class) {
                        markers.add(Zchars.Z_ANDTHEN);
                    }
                    return new ZscriptSequencePath(markers, findFirstCommand(next));
                }
            }

            if (current.getClass() == OrSeqElement.class && current.getParent() != null && current.getParent().getClass() != OrSeqElement.class) {
                markers.add(Zchars.Z_CLOSEPAREN);
            }
            prev = current;
            current = current.getParent();
        }
        return null;
    }

    public ZscriptSequencePath findFailPath() {
        CommandSeqElement current = parent;
        CommandSeqElement prev    = this;
        List<Byte>        markers = new ArrayList<>();

        while (current != null) {
            if (current.getClass() == OrSeqElement.class) {
                OrSeqElement orAncestor = (OrSeqElement) current;
                if (prev == orAncestor.before) {
                    markers.add(Zchars.Z_ORELSE);
                    return new ZscriptSequencePath(markers, findFirstCommand(orAncestor.after));
                }
            }
            if (current.getClass() == OrSeqElement.class && current.getParent() != null && current.getParent().getClass() != OrSeqElement.class) {
                markers.add(Zchars.Z_CLOSEPAREN);
            }
            prev = current;
            current = current.getParent();
        }
        return null;
    }

    public ZscriptSequencePath findSuccessPath() {
        CommandSeqElement current = parent;
        CommandSeqElement prev    = this;
        List<Byte>        markers = new ArrayList<>();

        while (current != null) {
            if (current.getClass() == AndSeqElement.class) {
                AndSeqElement andAncestor = (AndSeqElement) current;
                if (prev != andAncestor.getElements().get(andAncestor.getElements().size() - 1)) {
                    CommandSeqElement next = andAncestor.getElements().get(andAncestor.getElements().indexOf(prev) + 1);
                    if (prev.getClass() != OrSeqElement.class && next.getClass() != OrSeqElement.class) {
                        markers.add(Zchars.Z_ANDTHEN);
                    }
                    return new ZscriptSequencePath(markers, findFirstCommand(next));
                }
            }
            if (current.getClass() == OrSeqElement.class && current.getParent() != null && current.getParent().getClass() != OrSeqElement.class) {
                markers.add(Zchars.Z_CLOSEPAREN);
            }
            prev = current;
            current = current.getParent();
        }
        return null;
    }
}
