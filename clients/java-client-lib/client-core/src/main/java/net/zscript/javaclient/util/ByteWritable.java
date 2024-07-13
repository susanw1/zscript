package net.zscript.javaclient.util;

import net.zscript.javaclient.util.ZscriptByteString.ZscriptByteStringBuilder;

/**
 * Describes Zscript message components that may be written to a ZscriptByteStringBuilder, such as fields, addresses etc.
 */
public interface ByteWritable {
    /**
     * Defines how this writable object should actually be written.
     *
     * @param out the builder to write itself to
     * @return this ByteWritable, for method chaining
     */
    ByteWritable writeTo(final ZscriptByteStringBuilder out);
}
