package net.zscript.javareceiver.execution;

import net.zscript.javareceiver.core.Zscript;
import net.zscript.javareceiver.core.OutStream;

public interface ActionSource {
    ZscriptAction getAction();

    OutStream getOutStream(Zscript zscript);
}
