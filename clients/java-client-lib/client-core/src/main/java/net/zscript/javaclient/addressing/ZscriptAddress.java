package net.zscript.javaclient.addressing;

import java.util.Arrays;
import java.util.List;
import java.util.Optional;

import static net.zscript.javareceiver.tokenizer.TokenBuffer.TokenReader.ReadToken;

import net.zscript.model.components.Zchars;
import net.zscript.util.ByteString.ByteAppendable;
import net.zscript.util.ByteString.ByteStringBuilder;
import net.zscript.util.OptIterator;

/**
 * Representation of a single address, as per <code>@1.5.192</code>, where each element is a uint16.
 *
 * Aggregation to create a full multi-hop address is managed by {@link AddressedCommand} and {@link CompleteAddressedResponse}
 */
public class ZscriptAddress implements ByteAppendable {
    private final int[] addressParts;

    /**
     * Composes an Address from the supplied parts.
     *
     * @param addressParts the numeric 16-bit elements of the address, each in range 0-0xffff
     * @return a ZscriptAddress representing the supplied numeric parts
     * @throws IllegalArgumentException if any of the values is out of range 0-ffff
     */
    public static ZscriptAddress from(int... addressParts) {
        return new ZscriptAddress(addressParts.clone());
    }

    /**
     * Trivial synonym for {@link #from(int...)} which makes static imports more readable.
     */
    public static ZscriptAddress address(int... addressParts) {
        return from(addressParts);
    }

    /**
     * Composes an Address from the parts in the supplied list.
     *
     * @param addressParts the numeric 16-bit elements of the address, each in range 0-0xffff
     * @return a ZscriptAddress representing the supplied numeric parts
     * @throws IllegalArgumentException if any of the values is out of range 0-ffff
     */
    public static ZscriptAddress from(List<Integer> addressParts) {
        return new ZscriptAddress(addressParts.stream().mapToInt(i -> i).toArray());
    }

    public static ZscriptAddress parse(ReadToken token) {
        if (token.getKey() != Zchars.Z_ADDRESSING) {
            throw new IllegalArgumentException("Cannot parse address from non-address fields");
        }
        int length = 0;

        OptIterator<ReadToken> iter = token.getNextTokens();
        for (Optional<ReadToken> opt = iter.next(); opt.filter(t -> t.getKey() == Zchars.Z_ADDRESSING || t.getKey() == Zchars.Z_ADDRESSING_CONTINUE)
                .isPresent(); opt = iter.next()) {
            length++;
        }
        int                    i            = 0;
        int[]                  addrSections = new int[length];
        OptIterator<ReadToken> iter2        = token.getNextTokens();
        for (Optional<ReadToken> opt = iter2.next(); opt.filter(t -> t.getKey() == Zchars.Z_ADDRESSING || t.getKey() == Zchars.Z_ADDRESSING_CONTINUE)
                .isPresent(); opt = iter2.next()) {
            addrSections[i++] = opt.get().getData16();
        }
        return new ZscriptAddress(addrSections);
    }

    private ZscriptAddress(int[] addr) {
        for (int a : addr) {
            if (a < 0 || a > 0xffff) {
                throw new IllegalArgumentException("address " + Arrays.toString(addr) + " contains out of range element: " + a);
            }
        }
        this.addressParts = addr;
    }

    public int[] getAsInts() {
        return addressParts.clone();
    }

    public int size() {
        return addressParts.length;
    }

    @Override
    public int hashCode() {
        return Arrays.hashCode(addressParts);
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj) {
            return true;
        }
        if (obj == null) {
            return false;
        }
        if (!getClass().isInstance(obj)) {
            return false;
        }
        ZscriptAddress other = (ZscriptAddress) obj;
        return Arrays.equals(addressParts, other.addressParts);
    }

    /**
     * Presents this address in conventional "@3.6a.1" format, including empty field for zero.
     *
     * @return the address as a string
     */
    @Override
    public String toString() {
        return toByteString().asString();
    }

    @Override
    public void appendTo(ByteStringBuilder builder) {
        byte pre = Zchars.Z_ADDRESSING;
        for (int a : addressParts) {
            builder.appendByte(pre).appendNumeric16(a);
            pre = Zchars.Z_ADDRESSING_CONTINUE;
        }
    }

    public int getBufferLength() {
        int length = 0;
        for (int addressPart : addressParts) {
            if (addressPart == 0) {
                length += 2;
            } else if (addressPart < 0x100) {
                length += 3;
            } else {
                length += 4;
            }
        }
        return length;
    }
}
