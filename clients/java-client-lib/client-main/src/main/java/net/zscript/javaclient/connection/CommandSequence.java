package net.zscript.javaclient.connection;

import static net.zscript.model.components.Zchars.Z_NEWLINE;

import net.zscript.javaclient.commandbuilder.CommandSequenceNode;
import net.zscript.javaclient.commandbuilder.ZscriptByteString;
import net.zscript.model.components.Zchars;

public class CommandSequence extends ExpressionSequence<CommandSequence> {
    private final CommandSequenceNode rootNode;
    private       Integer             locks;

    public CommandSequence(CommandSequenceNode rootNode) {
        this.rootNode = rootNode;
    }

    public CommandSequence withLocks(int locks) {
        this.locks = locks;
        return this;
    }

    @Override
    public byte[] asZscriptBytes() {
        ZscriptByteString.ZscriptByteStringBuilder zbsb = ZscriptByteString.builder();
        getAddressPath().writeTo(zbsb);
        if (echoNumber != null) {
            zbsb.appendField(Zchars.Z_ECHO, echoNumber);
        }
        if (locks != null) {
            zbsb.appendField(Zchars.Z_LOCKS, locks);
        }
        zbsb.appendRaw(rootNode.compile());
        zbsb.appendByte(Z_NEWLINE);
        return zbsb.toByteArray();
    }

}
