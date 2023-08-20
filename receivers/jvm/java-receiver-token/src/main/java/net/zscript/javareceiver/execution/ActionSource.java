package net.zscript.javareceiver.execution;

import net.zscript.javareceiver.core.OutStream;
import net.zscript.javareceiver.core.Zscript;

public interface ActionSource {
    ZscriptAction getAction();

    OutStream getOutStream(Zscript zscript);
}
