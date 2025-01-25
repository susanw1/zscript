package net.zscript.javaclient.commandbuilderapi.nodes;

import javax.annotation.Nonnull;
import javax.annotation.Nullable;
import java.util.Collections;
import java.util.List;
import java.util.Map;

import net.zscript.javaclient.commandbuilderapi.Respondable;
import net.zscript.javaclient.commandbuilderapi.ZscriptResponse;
import net.zscript.javaclient.commandpaths.BigField;
import net.zscript.javaclient.commandpaths.ZscriptFieldSet;
import net.zscript.tokenizer.ZscriptExpression;
import net.zscript.util.ByteString.ByteStringBuilder;

public abstract class ZscriptCommandNode<T extends ZscriptResponse> extends CommandSequenceNode implements Respondable<T> {

    private final ResponseCaptor<T> captor;
    private final ZscriptFieldSet   fieldSet;

    protected ZscriptCommandNode(ZscriptCommandBuilder<T> builder) {
        this(builder.captor, List.copyOf(builder.bigFields), Map.copyOf(builder.fields));
    }

    protected ZscriptCommandNode() {
        this(null, Collections.emptyList(), Collections.emptyMap());
    }

    protected ZscriptCommandNode(@Nullable ResponseCaptor<T> captor, List<BigField> bigFields, Map<Byte, Integer> fields) {
        this.captor = captor;
        this.fieldSet = ZscriptFieldSet.fromMap(bigFields, fields);
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

    @Nonnull
    public abstract Class<T> getResponseType();

    @Nonnull
    public List<CommandSequenceNode> getChildren() {
        return Collections.emptyList();
    }

    @Nonnull
    public ZscriptFieldSet asFieldSet() {
        return fieldSet;
    }

    @Override
    public void appendTo(ByteStringBuilder b) {
        asFieldSet().appendTo(b);
    }
}
