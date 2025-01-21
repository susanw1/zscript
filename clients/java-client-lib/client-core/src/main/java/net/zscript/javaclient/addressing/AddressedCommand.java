package net.zscript.javaclient.addressing;

import java.util.ArrayDeque;
import java.util.Queue;

import net.zscript.javaclient.sequence.CommandSequence;
import net.zscript.model.components.Zchars;
import net.zscript.util.ByteString.ByteAppendable;
import net.zscript.util.ByteString.ByteStringBuilder;

/**
 * Represents a fixed outgoing command-sequence, which accumulates the addresses of routing nodes it passes through to get to the target.
 */
public class AddressedCommand implements ByteAppendable {
    private final Queue<ZscriptAddress> addressSections;
    private final CommandSequence       commandSequence;

    public AddressedCommand(CommandSequence commandSequence) {
        this.addressSections = new ArrayDeque<>();
        this.commandSequence = commandSequence;
    }

    /**
     * Adds an Address (eg '{@code @5.0.12}') to this command. FIXME: should this be front, or back of queue? Why a Queue?
     *
     * @param section the address section to add
     */
    public void addAddressLayer(ZscriptAddress section) {
        addressSections.add(section);
    }

    @Override
    public void appendTo(ByteStringBuilder builder) {
        builder.append(addressSections)
                .append(commandSequence)
                .appendByte(Zchars.Z_NEWLINE);
    }

    public CommandSequence getCommandSequence() {
        return commandSequence;
    }

    public boolean hasAddressLayer() {
        return !addressSections.isEmpty();
    }

    public int getBufferLength() {
        int length = commandSequence.getBufferLength();
        for (ZscriptAddress addr : addressSections) {
            length += addr.getBufferLength();
        }
        return length;
    }

    @Override
    public String toString() {
        return toStringImpl();
    }
}
