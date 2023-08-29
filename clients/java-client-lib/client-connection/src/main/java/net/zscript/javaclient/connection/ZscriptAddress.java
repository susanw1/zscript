package net.zscript.javaclient.connection;

import static java.util.Arrays.stream;
import static java.util.stream.Collectors.joining;

import java.util.Arrays;

/**
 * Simple encapsulation of a Zscript address.
 */
public class ZscriptAddress {
    private final int[] address;

    public static ZscriptAddress from(int... address) {
        return new ZscriptAddress(address);
    }

    private ZscriptAddress(int[] addr) {
        this.address = addr.clone();
    }

    public int[] getAsInts() {
        return address.clone();
    }

    @Override
    public int hashCode() {
        final int prime  = 31;
        int       result = 1;
        result = prime * result + Arrays.hashCode(address);
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
        if (!Arrays.equals(address, other.address)) {
            return false;
        }
        return true;
    }

    /**
     * Presents this address in conventional "@3.6a.1" format, including empty field for zero.
     *
     * @return the address as a string
     */
    @Override
    public String toString() {
        return stream(address)
                .mapToObj(a -> a == 0 ? "" : Integer.toHexString(a))
                .collect(joining(".", "@", ""));
    }
}
