package net.zscript.javaclient.connection;

import static java.util.Arrays.stream;
import static java.util.stream.Collectors.joining;

import java.util.Arrays;
import java.util.List;

/**
 * Simple encapsulation of a single Zscript address (ie the 1.2.3 or 4.5.6 part of an address pathway like @1.2.3@4.5.6 Z1).
 */
public class ZscriptAddress {
    private final int[] addressParts;

    public static ZscriptAddress from(int... addressParts) {
        return new ZscriptAddress(addressParts.clone());
    }

    public static ZscriptAddress from(List<Integer> addressParts) {
        return new ZscriptAddress(addressParts.stream().mapToInt(i -> i).toArray());
    }

    private ZscriptAddress(int[] addr) {
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
        final int prime  = 31;
        int       result = 1;
        result = prime * result + Arrays.hashCode(addressParts);
        return result;
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj) {
            return true;
        }
        if (obj == null) {
            return false;
        }
        if (getClass() != obj.getClass()) {
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
        return stream(addressParts)
                .mapToObj(a -> a == 0 ? "" : Integer.toHexString(a))
                .collect(joining(".", "@", ""));
    }
}
