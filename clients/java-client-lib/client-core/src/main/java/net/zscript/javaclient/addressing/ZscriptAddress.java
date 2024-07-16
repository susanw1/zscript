package net.zscript.javaclient.addressing;

import java.util.Arrays;
import java.util.List;

import static net.zscript.javareceiver.tokenizer.TokenBuffer.TokenReader.ReadToken;

import net.zscript.model.components.Zchars;
import net.zscript.util.ByteString.ByteAppendable;
import net.zscript.util.ByteString.ByteStringBuilder;

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

        // Note, tokenizer will only write one Address; subsequent addresses are inside the single token envelope.
        int[] parts = token.getNextTokens().stream()
                .takeWhile(t -> Zchars.isAddressing(t.getKey()))
                .mapToInt(ReadToken::getData16)
                .toArray();

        return new ZscriptAddress(parts);
    }

    private ZscriptAddress(int[] addr) {
        for (int a : addr) {
            if (a < 0 || a > 0xffff) {
                throw new IllegalArgumentException("address " + Arrays.toString(addr) + " contains out of range element: " + a);
            }
        }
        this.addressParts = addr;
    }

    /**
     * Determines the ints making up the address parts.
     *
     * @return a new array of address parts
     */
    public int[] getAsInts() {
        return addressParts.clone();
    }

    /**
     * Determines the number of address parts (eg @a.b.c has 3)
     *
     * @return the number of address parts in this address
     */
    public int size() {
        return addressParts.length;
    }

    @Override
    public final int hashCode() {
        return Arrays.hashCode(addressParts);
    }

    @Override
    public final boolean equals(Object obj) {
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

    /**
     * Defines how a ZscriptAddress should be written to a ByteString in canonical Zscript notation, eg "@12.34ab.c56"
     *
     * @param builder the builder to append to
     */
    @Override
    public void appendTo(ByteStringBuilder builder) {
        byte pre = Zchars.Z_ADDRESSING;
        for (int a : addressParts) {
            builder.appendByte(pre).appendNumeric16(a);
            pre = Zchars.Z_ADDRESSING_CONTINUE;
        }
    }

    /**
     * Length in bytes of the tokens in the token buffer for this whole address.
     *
     * @return token buffer space required, in bytes
     */
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
