package net.zscript.javareceiver.execution;

import net.zscript.javareceiver.core.Zcode;
import net.zscript.javareceiver.core.ZcodeOutStream;

public interface ActionSource {
    ZcodeAction getAction();

    ZcodeOutStream getOutStream(Zcode zcode);
}
