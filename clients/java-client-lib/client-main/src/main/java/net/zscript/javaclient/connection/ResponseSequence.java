package net.zscript.javaclient.connection;

import java.io.ByteArrayOutputStream;
import java.io.IOException;

import static net.zscript.javaclient.commandbuilder.Utils.formatField;
import static net.zscript.model.components.Zchars.Z_NEWLINE;

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
        try (ByteArrayOutputStream baos = new ByteArrayOutputStream()) {
            baos.write(formatField(Zchars.Z_RESPONSE_MARK, notificationId));
            getAddressPath().writeTo(baos);
            if (echoNumber != null) {
                baos.write(formatField(Zchars.Z_ECHO, echoNumber));
            }
            //            baos.write(rootNode.compile()); --- we need this, obvs!
            baos.write(Z_NEWLINE);
            return baos.toByteArray();
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }
}
