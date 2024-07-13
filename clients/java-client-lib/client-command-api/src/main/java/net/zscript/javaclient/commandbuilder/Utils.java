package net.zscript.javaclient.commandbuilder;

import java.nio.charset.StandardCharsets;
import java.util.BitSet;
import java.util.EnumSet;

import static java.util.stream.Collectors.toCollection;

import net.zscript.javaclient.commandbuilder.commandnodes.ZscriptCommandNode;

/**
 * These utility functions make up a toolkit invoked by the generated Java client Command Builders, such as subclasses of {@link ZscriptCommandNode}.
 *
 * @see ZscriptCommandNode
 */
public class Utils {
    /**
     * Converts a number representing an OR'ed set of bits into the corresponding values in an Enum, returned as an EnumSet. Bit 0 is the first value in the Enum, bit n is the 1<<n
     * value etc.
     *
     * @param bitFields an int representing a set of bits
     * @param enumClass the enum type to match
     * @param <E>       the enum type itself
     * @return a Set containing the corresponding values defined in the enum
     */
    public static <E extends Enum<E>> EnumSet<E> bitsetToEnumSet(int bitFields, Class<E> enumClass) {
        final E[] enumValues = enumClass.getEnumConstants();
        return BitSet.valueOf(new long[] { bitFields }).stream()
                .mapToObj(b -> enumValues[b])
                .collect(toCollection(() -> EnumSet.noneOf(enumClass)));
    }

    /**
     * Checks that the supplied number can index the supplied Enum type, and throws a ZscriptFieldOutOfRangeException if it isn't.
     *
     * @param value     the value to check
     * @param enumClass the class of the enum to validate against
     * @param key       the letter key of the field being checked, for error msg purposes
     * @param isBitset  true if this enum represents a bitset, where values are really powers of 2; false if it's a normal enum
     * @return the value passed in, for nesting
     * @throws ZscriptFieldOutOfRangeException if value is out of range
     */
    public static <E extends Enum<E>> int checkInEnumRange(int value, Class<E> enumClass, char key, boolean isBitset) {
        int max = enumClass.getEnumConstants().length;
        if (isBitset) {
            max = 1 << max;
        }
        if (value < 0 || value >= max) {
            throw new ZscriptFieldOutOfRangeException("name=%s, key='%c', value=0x%x, min=0x%x, max=0x%x", enumClass.getSimpleName(), key, value, 0, max - 1);
        }
        return value;
    }

    public static byte[] formatField(byte f, int value) {
        if (value >= 0x10000 || value < 0) {
            throw new IllegalStateException("Command fields must be uint16s: " + value);
        }
        if (value == 0) {
            return new byte[] { f };
        }
        if (value < 0x10) {
            return new byte[] { f, toHex(value) };
        }
        if (value < 0x100) {
            return new byte[] { f, toHex(value >> 4), toHex(value & 0xF) };
        }
        if (value < 0x1000) {
            return new byte[] { f, toHex(value >> 8), toHex((value >> 4) & 0xF), toHex(value & 0xF) };
        }
        return new byte[] { f, toHex(value >> 12), toHex((value >> 8) & 0xF), toHex((value >> 4) & 0xF), toHex(value & 0xF) };
    }

    /**
     * Simply converts a nibble value (0-15) to an ASCII hex char, lowercase.
     *
     * @param nibble a value 0-15
     * @return the corresponding hex character, '0'-'9' or 'a'-'f'
     * @throws IndexOutOfBoundsException if value is out of range
     */
    public static byte toHex(int nibble) {
        return HEX[nibble];
    }

    private static final byte[] HEX = "0123456789abcdef".getBytes(StandardCharsets.UTF_8);

}
