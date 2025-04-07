package net.zscript.javaclient.commandbuilderapi.defaultcommands;

import javax.annotation.Nonnull;

import net.zscript.javaclient.commandbuilderapi.nodes.ZscriptCommandBuilder;
import net.zscript.javaclient.commandbuilderapi.nodes.ZscriptCommandNode;
import net.zscript.javaclient.commandpaths.FieldElement;
import net.zscript.tokenizer.ZscriptExpression;
import net.zscript.util.ByteString;

public class GenericCommandBuilder extends ZscriptCommandBuilder<DefaultResponse> {
    @Override
    public FieldElement getFieldOrZero(byte key) {
        return super.getFieldOrZero(key);
    }

    @Override
    public FieldElement getFieldOrZero(char key) {
        return super.getFieldOrZero(key);
    }

    public GenericCommandBuilder setField(char key, int value) {
        return (GenericCommandBuilder) setField((byte) key, value);
    }

    public GenericCommandBuilder setFieldAsText(char key, ByteString data) {
        return (GenericCommandBuilder) setFieldAsText((byte) key, data);
    }

    public GenericCommandBuilder setFieldAsBytes(char key, ByteString data) {
        return (GenericCommandBuilder) setFieldAsBytes((byte) key, data);
    }

    public GenericCommandBuilder setFieldAsSmallest(char key, ByteString data) {
        return (GenericCommandBuilder) setFieldAsSmallest((byte) key, data);
    }

    @Nonnull
    @Override
    public ZscriptCommandNode<DefaultResponse> build() {
        return new ZscriptCommandNode<>(this) {
            @Nonnull
            @Override
            public DefaultResponse parseResponse(ZscriptExpression response) {
                return new DefaultResponse(response);
            }

            @Nonnull
            @Override
            public Class<DefaultResponse> getResponseType() {
                return DefaultResponse.class;
            }
        };
    }
}
