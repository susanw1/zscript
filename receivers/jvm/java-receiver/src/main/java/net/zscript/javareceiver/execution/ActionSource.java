package net.zscript.javareceiver.execution;

import net.zscript.javareceiver.core.SequenceOutStream;
import net.zscript.javareceiver.core.Zscript;

public interface ActionSource {
    ZscriptAction getAction();

    SequenceOutStream getOutStream(Zscript zscript);
}
