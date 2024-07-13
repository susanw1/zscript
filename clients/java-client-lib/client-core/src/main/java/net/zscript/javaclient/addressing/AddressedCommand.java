package net.zscript.javaclient.addressing;

import java.util.ArrayDeque;
import java.util.Queue;

import net.zscript.javaclient.sequence.CommandSequence;
import net.zscript.javaclient.util.ZscriptByteString;
import net.zscript.util.ByteString;

public class AddressedCommand {
    private final Queue<ZscriptAddress> addressSections;
    private final CommandSequence       content;

    public AddressedCommand(CommandSequence content) {
        this.addressSections = new ArrayDeque<>();
        this.content = content;
    }

    public void addAddressLayer(ZscriptAddress section) {
        addressSections.add(section);
    }

    public ByteString toBytes() {
        ZscriptByteString.ZscriptByteStringBuilder builder = ZscriptByteString.builder();

        builder.append(addressSections);
        content.toBytes(builder);
        builder.appendByte('\n');
        return builder.build();
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
