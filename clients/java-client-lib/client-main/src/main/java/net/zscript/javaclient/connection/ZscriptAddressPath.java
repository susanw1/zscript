package net.zscript.javaclient.connection;

import java.util.List;
import java.util.Objects;

import static java.util.List.copyOf;
import static java.util.Objects.requireNonNull;
import static java.util.stream.Collectors.joining;

import net.zscript.javaclient.commandbuilder.ByteWritable;
import net.zscript.javaclient.commandbuilder.ZscriptByteString.ZscriptByteStringBuilder;

/**
 * Simple encapsulation of a set of Zscript addresses, representing the Zscript location of a device.
 */
public final class ZscriptAddressPath implements ByteWritable {
    private final List<ZscriptAddress> addresses;

    public static ZscriptAddressPath path(ZscriptAddress... addresses) {
        return from(addresses);
    }

    /**
     * Composes an Address Path from the supplied components.
     *
     * @param addresses a list of addresses
     * @return
     */
    public static ZscriptAddressPath from(ZscriptAddress... addresses) {
        return new ZscriptAddressPath(List.of(addresses));
    }

    /**
     * Trivial synonym for {@link #from(ZscriptAddress...)} which makes static imports more readable.
     */
    public static ZscriptAddressPath path(List<ZscriptAddress> addressParts) {
        return from(addressParts);
    }

    public static ZscriptAddressPath from(List<ZscriptAddress> addressParts) {
        return new ZscriptAddressPath(addressParts);
    }

    private ZscriptAddressPath(List<ZscriptAddress> addresses) {
        this.addresses = copyOf(requireNonNull(addresses));
    }

    public List<ZscriptAddress> addresses() {
        return addresses;
    }

    public int size() {
        return addresses.size();
    }

    @Override
    public int hashCode() {
        return Objects.hashCode(addresses);
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
        ZscriptAddressPath other = (ZscriptAddressPath) obj;
        return Objects.equals(addresses, other.addresses);
    }

    /**
     * Presents this path in conventional @1.2.3@4.5.6 format.
     *
     * @return the address path as a string
     */
    @Override
    public String toString() {
        return addresses.stream().map(ZscriptAddress::toString).collect(joining());
    }

    @Override
    public ZscriptAddressPath writeTo(ZscriptByteStringBuilder out) {
        for (ZscriptAddress a : addresses) {
            a.writeTo(out);
        }
        return this;
    }
}
