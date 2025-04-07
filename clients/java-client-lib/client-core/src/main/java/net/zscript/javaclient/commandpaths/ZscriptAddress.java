package net.zscript.javaclient.commandpaths;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;

import static java.util.stream.Collectors.toList;
import static net.zscript.javaclient.commandpaths.FieldElement.fieldOf;
import static net.zscript.tokenizer.TokenBuffer.TokenReader.ReadToken;

import net.zscript.model.components.Zchars;
import net.zscript.util.ByteString.ByteAppendable;
import net.zscript.util.ByteString.ByteStringBuilder;

/**
 * Representation of a single address, as per <code>@1.5.192</code>, where each element is a uint16.
 * <p>Aggregation to create a full multi-hop response address is managed by {@link CompleteAddressedResponse}
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

    public static List<ZscriptAddress> parseAll(ReadToken start) {
        if (start.getKey() != Zchars.Z_ADDRESSING) {
            return Collections.emptyList();
        }

        final List<List<Integer>> addresses = new ArrayList<>();
        List<Integer>             elements  = null;

        for (ReadToken token : start.tokenIterator().toIterable()) {
            if (token.getKey() == Zchars.Z_ADDRESSING) {
                elements = new ArrayList<>();
                addresses.add(elements);
                elements.add(token.getData16());
            } else if (token.getKey() == Zchars.Z_ADDRESSING_CONTINUE) {
                elements.add(token.getData16());
            } else {
                break;
            }
        }

        return addresses.stream()
                .map(list -> new ZscriptAddress(list.stream()
                        .mapToInt(i -> i)
                        .toArray()))
                .collect(toList());
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

    @Override
    public String toString() {
        return toStringImpl();
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
            fieldOf(pre, a).appendTo(builder);
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
