package net.zscript.javaclient.commandbuilder.commandnodes;

import javax.annotation.Nonnull;
import javax.annotation.Nullable;
import java.util.Collections;
import java.util.List;
import java.util.Map;
import java.util.stream.Collectors;

import static net.zscript.javaclient.commandPaths.NumberField.fieldOf;

import net.zscript.javaclient.commandPaths.BigField;
import net.zscript.javaclient.commandPaths.ZscriptFieldSet;
import net.zscript.javaclient.commandbuilder.Respondable;
import net.zscript.javaclient.commandbuilder.ZscriptResponse;
import net.zscript.tokenizer.ZscriptExpression;
import net.zscript.util.ByteString;

public abstract class ZscriptCommandNode<T extends ZscriptResponse> extends CommandSequenceNode implements Respondable<T> {

    private final ResponseCaptor<T> captor;

    private final List<BigField>     bigFields;
    private final Map<Byte, Integer> fields;

    protected ZscriptCommandNode(ZscriptCommandBuilder<T> builder) {
        this(builder.captor, List.copyOf(builder.bigFields), Map.copyOf(builder.fields));
    }

    protected ZscriptCommandNode() {
        this(null, Collections.emptyList(), Collections.emptyMap());
    }

    protected ZscriptCommandNode(@Nullable ResponseCaptor<T> captor, List<BigField> bigFields, Map<Byte, Integer> fields) {
        this.captor = captor;
        this.bigFields = bigFields;
        this.fields = fields;
        if (captor != null) {
            captor.setSource(this);
        }
    }

    @Nullable
    public ResponseCaptor<T> getCaptor() {
        return captor;
    }

    @Nonnull
    public abstract T parseResponse(ZscriptExpression response);

    public abstract Class<T> getResponseType();

    public List<CommandSequenceNode> getChildren() {
        return Collections.emptyList();
    }

    public ZscriptFieldSet asFieldSet() {
        return ZscriptFieldSet.fromMap(bigFields.stream().map(BigField::getData).collect(Collectors.toList()),
                bigFields.stream().map(BigField::isString).collect(Collectors.toList()), fields);
    }

    @Override
    public String asString() {
        ByteString.ByteStringBuilder b = ByteString.builder();
        for (int i = 'A'; i <= 'Z'; i++) {
            if (fields.get((byte) i) != null) {
                int value = fields.get((byte) i);
                fieldOf((byte) i, value).appendTo(b);
            }
        }
        for (BigField f : bigFields) {
            f.appendTo(b);
        }
        return b.asString();
    }
}
