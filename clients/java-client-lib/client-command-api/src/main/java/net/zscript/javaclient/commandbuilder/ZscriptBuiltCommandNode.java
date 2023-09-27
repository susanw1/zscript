package net.zscript.javaclient.commandbuilder;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.UncheckedIOException;
import java.util.List;
import java.util.Map;

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
        try {
            ByteArrayOutputStream out = new ByteArrayOutputStream();
            if (fields.get(Zchars.Z_CMD) != null) {
                out.write(Utils.formatField(Zchars.Z_CMD, fields.get(Zchars.Z_CMD)));
            }
            for (Map.Entry<Byte, Integer> entry : fields.entrySet()) {
                Byte key = entry.getKey();
                if (key != Zchars.Z_CMD) {
                    Integer val = entry.getValue();
                    out.write(Utils.formatField(key, val));
                }
            }
            for (BigField big : bigFields) {
                big.write(out);
            }
            return out.toByteArray();
        } catch (IOException e) {
            throw new UncheckedIOException(e);
        }
    }

    @Override
    public void onResponse(ZscriptExpression resp) {
        T parsed = parseResponse(resp);
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
}
