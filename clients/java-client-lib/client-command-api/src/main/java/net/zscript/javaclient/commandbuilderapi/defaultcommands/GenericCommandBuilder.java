package net.zscript.javaclient.commandbuilderapi.defaultcommands;

import javax.annotation.Nonnull;

import net.zscript.javaclient.commandbuilderapi.nodes.ZscriptCommandBuilder;
import net.zscript.javaclient.commandbuilderapi.nodes.ZscriptCommandNode;
import net.zscript.tokenizer.ZscriptExpression;

public class GenericCommandBuilder extends ZscriptCommandBuilder<DefaultResponse> {
    @Override
    public int getField(byte key) {
        return super.getField(key);
    }

    @Override
    public int getField(char key) {
        return super.getField(key);
    }

    public GenericCommandBuilder setField(char key, int value) {
        return (GenericCommandBuilder) setField((byte) key, value);
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
