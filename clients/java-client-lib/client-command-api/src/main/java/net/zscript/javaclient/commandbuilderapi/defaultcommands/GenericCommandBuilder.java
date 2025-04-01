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

    @Override
    public GenericCommandBuilder addBigField(byte[] data) {
        return (GenericCommandBuilder) super.addBigField(data);
    }

    @Override
    public GenericCommandBuilder addBigField(byte[] data, boolean asString) {
        return (GenericCommandBuilder) super.addBigField(data, asString);
    }

    @Override
    public GenericCommandBuilder addBigFieldAsSmallest(byte[] data) {
        return (GenericCommandBuilder) super.addBigFieldAsSmallest(data);
    }

    @Override
    public GenericCommandBuilder addBigField(String text) {
        return (GenericCommandBuilder) super.addBigField(text);
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
