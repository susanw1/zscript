package net.zscript.javaclient.connection;

import static net.zscript.model.components.Zchars.Z_NEWLINE;

import net.zscript.javaclient.commandbuilder.ZscriptByteString;
import net.zscript.javaclient.commandbuilder.ZscriptByteString.ZscriptByteStringBuilder;
import net.zscript.model.components.Zchars;

public class ResponseSequence extends ExpressionSequence<ResponseSequence> {
    private int notificationId;

    ResponseSequence() {
    }

    public ResponseSequence withNotificationId(int notificationId) {
        this.notificationId = notificationId;
        return this;
    }

    public byte[] asZscriptBytes() {
        ZscriptByteStringBuilder zbsb = ZscriptByteString.builder();
        zbsb.appendField(Zchars.Z_RESPONSE_MARK, notificationId);
        getAddressPath().writeTo(zbsb);
        if (echoNumber != null) {
            zbsb.appendField(Zchars.Z_ECHO, echoNumber);
        }
        //        zbsb.appendRaw(rootNode.compile());
        zbsb.appendByte(Z_NEWLINE);
        return zbsb.toByteArray();
    }
}
