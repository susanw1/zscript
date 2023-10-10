package net.zscript.javaclient.commandbuilder;

import java.util.List;
import java.util.Map;

import net.zscript.javaclient.commandbuilder.ZscriptByteString.ZscriptByteStringBuilder;
import net.zscript.javaclient.commandbuilder.ZscriptCommandBuilder.BigField;
import net.zscript.javareceiver.tokenizer.ZscriptExpression;
import net.zscript.model.components.Zchars;

/**
 * A representation of a command, returned by calling {@link ZscriptCommandBuilder#build()}, ready for amalgamation into a command sequence.
 */
public abstract class ZscriptBuiltCommandNode<T extends ZscriptResponse> extends ZscriptCommandNode {
    private final List<ZscriptResponseListener<T>> listeners;
    private final List<BigField>                   bigFields;
    private final Map<Byte, Integer>               fields;

    protected ZscriptBuiltCommandNode(ZscriptCommandBuilder<T> builder) {
        this.listeners = List.copyOf(builder.listeners);
        this.bigFields = List.copyOf(builder.bigFields);
        this.fields = (builder.fields);
        //        this.fields = Map.copyOf(builder.fields);
    }

    protected abstract T parseResponse(ZscriptExpression resp);

    @Override
    byte[] compile(boolean includeParens) {
        ZscriptByteStringBuilder bsb = ZscriptByteString.builder();
        if (fields.get(Zchars.Z_CMD) != null) {
            bsb.appendField(Zchars.Z_CMD, fields.get(Zchars.Z_CMD));
        }
        for (Map.Entry<Byte, Integer> entry : fields.entrySet()) {
            Byte key = entry.getKey();
            if (key != Zchars.Z_CMD) {
                Integer val = entry.getValue();
                bsb.appendField(key, val);
            }
        }
        for (BigField big : bigFields) {
            big.writeTo(bsb);
        }
        return bsb.toByteArray();
    }

    @Override
    public void onResponse(ZscriptExpression response) {
        T parsed = parseResponse(response);
        for (ZscriptResponseListener<T> listener : listeners) {
            listener.accept(parsed);
        }
    }

    @Override
    public void onNotExecuted() {
        for (ZscriptResponseListener<T> listener : listeners) {
            listener.notExecuted();
        }
    }

    public Map<Byte, Integer> getFields() {
        return fields;
    }

    public List<BigField> getBigFields() {
        return bigFields;
    }
}
