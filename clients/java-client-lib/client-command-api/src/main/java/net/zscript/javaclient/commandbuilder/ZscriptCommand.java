package net.zscript.javaclient.commandbuilder;

import java.util.ArrayList;
import java.util.List;

public abstract class ZscriptCommand extends CommandSeqElement {
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

    public abstract void notExecuted();

    public abstract void response(ZscriptUnparsedCommandResponse resp);

    public static ZscriptSequencePath findFirstCommand(final CommandSeqElement start) {
        CommandSeqElement current = start;
        CommandSeqElement prev    = null;
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
                markers.add((byte) '(');
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
                    markers.add((byte) '|');
                    return new ZscriptSequencePath(markers, findFirstCommand(orAncestor.after));
                }
            } else if (current.getClass() == AndSeqElement.class) {
                AndSeqElement andAncestor = (AndSeqElement) current;
                if (prev != andAncestor.getElements().get(andAncestor.getElements().size() - 1)) {
                    CommandSeqElement next = andAncestor.getElements().get(andAncestor.getElements().indexOf(prev) + 1);
                    if (prev.getClass() != OrSeqElement.class && next.getClass() != OrSeqElement.class) {
                        markers.add((byte) '&');
                    }
                    return new ZscriptSequencePath(markers, findFirstCommand(next));
                }
            }
            if (current.getClass() == OrSeqElement.class && current.getParent() != null && current.getParent().getClass() != OrSeqElement.class) {
                markers.add((byte) ')');
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
                    markers.add((byte) '|');
                    return new ZscriptSequencePath(markers, findFirstCommand(orAncestor.after));
                }
            }
            if (current.getClass() == OrSeqElement.class && current.getParent() != null && current.getParent().getClass() != OrSeqElement.class) {
                markers.add((byte) ')');
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
                        markers.add((byte) '&');
                    }
                    return new ZscriptSequencePath(markers, findFirstCommand(next));
                }
            }
            if (current.getClass() == OrSeqElement.class && current.getParent() != null && current.getParent().getClass() != OrSeqElement.class) {
                markers.add((byte) ')');
            }
            prev = current;
            current = current.getParent();
        }
        return null;
    }
}
