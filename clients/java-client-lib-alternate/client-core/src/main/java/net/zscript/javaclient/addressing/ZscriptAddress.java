package net.zscript.javaclient.addressing;

import static java.nio.charset.StandardCharsets.UTF_8;
import static net.zscript.javareceiver.tokenizer.TokenBuffer.TokenReader.ReadToken;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.UncheckedIOException;
import java.util.Arrays;
import java.util.List;
import java.util.Optional;

import net.zscript.javaclient.commandbuilder.ByteWritable;
import net.zscript.javaclient.commandbuilder.ZscriptByteString;
import net.zscript.javareceiver.tokenizer.TokenBuffer;
import net.zscript.model.components.Zchars;
import net.zscript.util.OptIterator;

public class ZscriptAddress implements ByteWritable {
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
        try (ByteArrayOutputStream baos = new ByteArrayOutputStream()) {
            final ZscriptByteString.ZscriptByteStringBuilder builder = ZscriptByteString.builder();
            writeTo(builder);
            builder.writeTo(baos);
            return baos.toString(UTF_8);
        } catch (IOException e) {
            // this cannot happen with a BAOS, but hey.
            throw new UncheckedIOException(e);
        }
    }

    @Override
    public ZscriptAddress writeTo(final ZscriptByteString.ZscriptByteStringBuilder out) {
        byte pre = Zchars.Z_ADDRESSING;
        for (int a : addressParts) {
            out.appendField(pre, a);
            pre = Zchars.Z_ADDRESSING_CONTINUE;
        }
        return this;
    }
}
