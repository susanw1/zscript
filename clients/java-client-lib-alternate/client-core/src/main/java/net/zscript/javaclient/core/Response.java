package net.zscript.javaclient.core;

import java.io.ByteArrayOutputStream;
import java.io.OutputStream;
import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;

public class Response {
    private final ZscriptFieldSet fieldSet;
    private final Response        next;
    private final boolean         wasSuccess;

    Response(Response next, boolean wasSuccess, ZscriptFieldSet fieldSet) {
        this.next = next;
        this.wasSuccess = wasSuccess;
        this.fieldSet = fieldSet;
    }

    public boolean wasSuccess() {
        return wasSuccess;
    }

    public Response getNext() {
        return next;
    }

    public void toBytes(OutputStream out) {
        fieldSet.toBytes(out);
    }

    @Override
    public String toString() {
        ByteArrayOutputStream outputStream = new ByteArrayOutputStream();
        toBytes(outputStream);
        return StandardCharsets.UTF_8.decode(ByteBuffer.wrap(outputStream.toByteArray())).toString();
    }

    public ZscriptFieldSet getFields() {
        return fieldSet;
    }
}
