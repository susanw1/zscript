package net.zscript.javaclient.addressing;

import java.util.ArrayDeque;
import java.util.Queue;

import net.zscript.javaclient.sequence.CommandSequence;
import net.zscript.model.components.Zchars;
import net.zscript.util.ByteString.ByteAppendable;
import net.zscript.util.ByteString.ByteStringBuilder;

public class AddressedCommand implements ByteAppendable {
    private final Queue<ZscriptAddress> addressSections;
    private final CommandSequence       content;

    public AddressedCommand(CommandSequence content) {
        this.addressSections = new ArrayDeque<>();
        this.content = content;
    }

    public void addAddressLayer(ZscriptAddress section) {
        addressSections.add(section);
    }

    @Override
    public void appendTo(ByteStringBuilder builder) {
        builder.append(addressSections)
                .append(content)
                .appendByte(Zchars.Z_NEWLINE);
    }

    public CommandSequence getContent() {
        return content;
    }

    public boolean hasAddressLayer() {
        return !addressSections.isEmpty();
    }

    public int getBufferLength() {
        int length = content.getBufferLength();
        for (ZscriptAddress addr : addressSections) {
            length += addr.getBufferLength();
        }
        return length;
    }
}
