package net.zscript.javaclient.commandPaths;


import net.zscript.javaclient.commandbuilder.ZscriptByteString;

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

    public void toBytes(ZscriptByteString.ZscriptByteStringBuilder out) {
        fieldSet.toBytes(out);
    }

    @Override
    public String toString() {
        ZscriptByteString.ZscriptByteStringBuilder out = ZscriptByteString.builder();
        toBytes(out);
        return out.asString();
    }

    public ZscriptFieldSet getFields() {
        return fieldSet;
    }
}
