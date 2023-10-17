package net.zscript.javaclient;

import net.zscript.javaclient.ZscriptByteString.ZscriptByteStringBuilder;

public interface ByteWritable {
    ByteWritable writeTo(final ZscriptByteStringBuilder out);
}
