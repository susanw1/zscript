package net.zscript.javaclient.commandbuilder.commandnodes;

import java.util.Arrays;
import java.util.Collections;
import java.util.List;
import java.util.Map;
import java.util.stream.Collectors;

import net.zscript.javaclient.commandPaths.ZscriptFieldSet;
import net.zscript.javaclient.ZscriptByteString;
import net.zscript.javaclient.commandbuilder.ZscriptResponse;
import net.zscript.javaclient.commandbuilder.commandnodes.ZscriptCommandBuilder.BigField;
import net.zscript.javareceiver.tokenizer.ZscriptExpression;

public abstract class ZscriptCommandNode<T extends ZscriptResponse> extends CommandSequenceNode {

    private final ResponseCaptor<T> captor;

    private final List<BigField>     bigFields;
    private final Map<Byte, Integer> fields;

    protected ZscriptCommandNode(ZscriptCommandBuilder<T> builder) {
        this(builder.captor, List.copyOf(builder.bigFields), Map.copyOf(builder.fields));
    }

    protected ZscriptCommandNode() {
        this(null, Collections.emptyList(), Collections.emptyMap());
    }

    protected ZscriptCommandNode(ResponseCaptor<T> captor, List<BigField> bigFields, Map<Byte, Integer> fields) {
        this.captor = captor;
        this.bigFields = bigFields;
        this.fields = fields;
        if (captor != null) {
            captor.setCommand(this);
        }
    }

    public abstract T parseResponse(ZscriptExpression response);

    public abstract Class<T> getResponseType();

    public List<CommandSequenceNode> getChildren() {
        return Collections.emptyList();
    }

    public void responseArrived(ZscriptResponse response) {
        if (captor != null) {
            captor.responseReceived(getResponseType().cast(response));
        }
    }

    public void resetResponseParsing() {
        if (captor != null) {
            captor.resetResponseParsing();
        }
    }

    public ZscriptFieldSet asFieldSet() {
        return ZscriptFieldSet.fromMap(bigFields.stream().map(BigField::getData).collect(Collectors.toList()),
                bigFields.stream().map(BigField::isString).collect(Collectors.toList()), fields);
    }

    @Override
    public String asString() {
        ZscriptByteString.ZscriptByteStringBuilder b = ZscriptByteString.builder();
        for (int i = 'A'; i <= 'Z'; i++) {
            if (fields.get((byte) i) != null) {
                b.appendField((byte) i, fields.get((byte) i));
            }
        }
        for (BigField f : bigFields) {
            f.writeTo(b);
        }
        return b.asString();
    }
}
