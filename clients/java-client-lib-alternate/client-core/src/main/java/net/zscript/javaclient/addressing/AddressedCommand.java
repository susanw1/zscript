package net.zscript.javaclient.addressing;

import java.util.ArrayDeque;
import java.util.Queue;

import net.zscript.javaclient.commandbuilder.ZscriptByteString;
import net.zscript.javaclient.sequence.CommandSequence;
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
        toBytes(builder);
        return builder.build();
    }

    public void toBytes(ZscriptByteString.ZscriptByteStringBuilder builder) {
        for (ZscriptAddress address : addressSections) {
            address.toBytes(builder);
        }
        content.toBytes(builder);
    }
}
