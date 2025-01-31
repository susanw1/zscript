package net.zscript.javaclient.devicenodes;

import java.util.LinkedList;
import java.util.List;

import static java.util.List.copyOf;

import net.zscript.javaclient.commandpaths.ZscriptAddress;
import net.zscript.javaclient.sequence.CommandSequence;
import net.zscript.model.components.Zchars;
import net.zscript.util.ByteString.ByteAppendable;
import net.zscript.util.ByteString.ByteStringBuilder;

/**
 * Represents a fixed outgoing command-sequence, along with the addresses of routing nodes it passes through to get to the target.
 */
public class AddressedCommand implements ByteAppendable {
    private final LinkedList<ZscriptAddress> addressSections;
    private final CommandSequence            commandSequence;
    private final int                        sequenceTokensLength;

    public AddressedCommand(CommandSequence commandSequence) {
        this.addressSections = new LinkedList<>();
        this.commandSequence = commandSequence;
        this.sequenceTokensLength = commandSequence.getBufferLength();
    }

    /**
     * Prefixes an Address (eg '{@code @5.0.12}') to this command.
     *
     * @param section the address section to add to the front of the address
     */
    public void prefixAddressLayer(ZscriptAddress section) {
        addressSections.addFirst(section);
    }

    /**
     * Writes the complete Zscript message, including addresses and terminating newline, to the specified byteString builder.
     *
     * @param builder the builder to append to
     */
    @Override
    public void appendTo(ByteStringBuilder builder) {
        builder.append(addressSections)
                .append(commandSequence)
                .appendByte(Zchars.Z_NEWLINE);
    }

    public CommandSequence getCommandSequence() {
        return commandSequence;
    }

    public boolean hasAddress() {
        return !addressSections.isEmpty();
    }

    public List<ZscriptAddress> getAddressSections() {
        return copyOf(addressSections);
    }

    public int getBufferLength() {
        int length = sequenceTokensLength;
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
