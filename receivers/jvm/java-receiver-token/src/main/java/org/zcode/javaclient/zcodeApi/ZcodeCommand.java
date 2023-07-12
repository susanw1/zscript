package org.zcode.javaclient.zcodeApi;

import java.util.ArrayList;
import java.util.List;

public abstract class ZcodeCommand extends CommandSeqElement {
    public static class ZcodeSequencePath {
        private final List<Byte>   markers;
        private final ZcodeCommand next;

        public ZcodeSequencePath(List<Byte> markers, ZcodeCommand next) {
            this.markers = markers;
            this.next = next;
        }

        public ZcodeSequencePath(List<Byte> markers, ZcodeSequencePath rest) {
            this.markers = new ArrayList<>(markers);
            this.markers.addAll(rest.markers);
            this.next = rest.next;
        }

        public List<Byte> getMarkers() {
            return markers;
        }

        public ZcodeCommand getNext() {
            return next;
        }
    }

    public abstract void response(ZcodeUnparsedCommandResponse resp);

    public static ZcodeSequencePath findFirstCommand(final CommandSeqElement start) {
        CommandSeqElement current = start;
        CommandSeqElement prev    = null;
        List<Byte>        markers = new ArrayList<>();
        while (!current.isCommand()) {
            if (current.getClass() == ZcodeAndSeqElement.class) {
                prev = current;
                current = ((ZcodeAndSeqElement) current).getElements().get(0);
            } else if (current.getClass() == ZcodeOrSeqElement.class) {
                prev = current;
                current = ((ZcodeOrSeqElement) current).before;
            } else {
                throw new IllegalStateException("Unknown CommandSeqElement: " + current);
            }
            if (current.getClass() == ZcodeOrSeqElement.class && prev.getClass() == ZcodeOrSeqElement.class) {
                markers.add((byte) '(');
            }
        }
        return new ZcodeSequencePath(markers, (ZcodeCommand) current);
    }

    public ZcodeSequencePath findFailPath() {
        CommandSeqElement current = parent;
        CommandSeqElement prev    = this;
        List<Byte>        markers = new ArrayList<>();

        while (current != null) {
            if (current.getClass() == ZcodeOrSeqElement.class) {
                ZcodeOrSeqElement orAncestor = (ZcodeOrSeqElement) current;
                if (prev == orAncestor.before) {
                    markers.add((byte) '|');
                    return new ZcodeSequencePath(markers, findFirstCommand(orAncestor.after));
                }
            }
            if (current.getClass() == ZcodeOrSeqElement.class && current.getParent() != null && current.getParent().getClass() != ZcodeOrSeqElement.class) {
                markers.add((byte) ')');
            }
            prev = current;
            current = current.getParent();
        }
        return null;
    }

    public ZcodeSequencePath findSuccessPath() {
        CommandSeqElement current = parent;
        CommandSeqElement prev    = this;
        List<Byte>        markers = new ArrayList<>();

        while (current != null) {
            if (current.getClass() == ZcodeAndSeqElement.class) {
                ZcodeAndSeqElement andAncestor = (ZcodeAndSeqElement) current;
                if (prev != andAncestor.getElements().get(andAncestor.getElements().size() - 1)) {
                    CommandSeqElement next = andAncestor.getElements().get(andAncestor.getElements().indexOf(prev) + 1);
                    if (prev.getClass() != ZcodeOrSeqElement.class && next.getClass() != ZcodeOrSeqElement.class) {
                        markers.add((byte) '&');
                    }
                    return new ZcodeSequencePath(markers, findFirstCommand(next));
                }
            }
            if (current.getClass() == ZcodeOrSeqElement.class && current.getParent() != null && current.getParent().getClass() != ZcodeOrSeqElement.class) {
                markers.add((byte) ')');
            }
            prev = current;
            current = current.getParent();
        }
        return null;
    }
}
