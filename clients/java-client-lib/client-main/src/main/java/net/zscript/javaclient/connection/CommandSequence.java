package net.zscript.javaclient.connection;

import static net.zscript.javaclient.commandbuilder.ZscriptCommandBuilder.formatField;
import static net.zscript.model.components.Zchars.Z_NEWLINE;

import java.io.ByteArrayOutputStream;
import java.io.IOException;

import net.zscript.javaclient.commandbuilder.CommandSequenceNode;
import net.zscript.model.components.Zchars;

public class CommandSequence extends ExpressionSequence<CommandSequence> {
    private Integer             locks;
    private CommandSequenceNode rootNode;

    public CommandSequence(CommandSequenceNode rootNode) {
        this.rootNode = rootNode;
    }

    public CommandSequence withLocks(int locks) {
        this.locks = locks;
        return this;
    }

    @Override
    public byte[] asZscriptBytes() {
        try (ByteArrayOutputStream baos = new ByteArrayOutputStream()) {
            getAddressPath().writeTo(baos);
            if (echoNumber != null) {
                baos.write(formatField(Zchars.Z_ECHO, echoNumber));
            }
            if (locks != null) {
                baos.write(formatField(Zchars.Z_LOCKS, locks));
            }
            baos.write(rootNode.compile());
            baos.write(Z_NEWLINE);
            return baos.toByteArray();
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

}
