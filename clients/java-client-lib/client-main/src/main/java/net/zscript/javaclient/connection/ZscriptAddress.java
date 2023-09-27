package net.zscript.javaclient.connection;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.io.UncheckedIOException;
import java.util.Arrays;
import java.util.List;

import static java.lang.Integer.toHexString;
import static java.nio.charset.StandardCharsets.UTF_8;

import net.zscript.javaclient.commandbuilder.ByteWritable;
import net.zscript.model.components.Zchars;

/**
 * Simple encapsulation of a single Zscript address (ie the 1.2.3 or 4.5.6 part of an address pathway like @1.2.3@4.5.6 Z1).
 */
public final class ZscriptAddress implements ByteWritable {
    private final int[] addressParts;

    /**
     * Composes an Address from the
     *
     * @param addressParts
     * @return
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

    public static ZscriptAddress from(List<Integer> addressParts) {
        return new ZscriptAddress(addressParts.stream().mapToInt(i -> i).toArray());
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
        try (ByteArrayOutputStream baos = new ByteArrayOutputStream()) {
            return writeTo(baos).toString(UTF_8);
        } catch (IOException e) {
            // this cannot happen with a BAOS, but hey.
            throw new UncheckedIOException(e);
        }
    }

    @Override
    public <T extends OutputStream> T writeTo(final T out) throws IOException {
        byte pre = Zchars.Z_ADDRESSING;
        for (int a : addressParts) {
            out.write(pre);
            pre = Zchars.Z_ADDRESSING_CONTINUE;
            if (a != 0) {
                out.write(toHexString(a).getBytes(UTF_8));
            }
        }
        return out;
    }
}
