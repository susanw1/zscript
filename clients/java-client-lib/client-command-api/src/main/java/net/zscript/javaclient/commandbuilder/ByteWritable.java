package net.zscript.javaclient.commandbuilder;

import net.zscript.javaclient.commandbuilder.ZscriptByteString.ZscriptByteStringBuilder;

public interface ByteWritable {
    ByteWritable writeTo(final ZscriptByteStringBuilder out);
}
