package net.zscript.javaclient.commandbuilderapi.defaultcommands;

import javax.annotation.Nonnull;

import net.zscript.javaclient.commandbuilderapi.nodes.ZscriptCommandBuilder;
import net.zscript.javaclient.commandbuilderapi.nodes.ZscriptCommandNode;
import net.zscript.tokenizer.ZscriptExpression;

public class GenericCommandBuilder extends ZscriptCommandBuilder<DefaultResponse> {
    public int getField(byte key) {
        return super.getField(key);
    }

    public int getField(char key) {
        return super.getField(key);
    }

    public ZscriptCommandBuilder<DefaultResponse> addBigField(byte[] data) {
        return super.addBigField(data);
    }

    public ZscriptCommandBuilder<DefaultResponse> addBigField(byte[] data, boolean asString) {
        return super.addBigField(data, asString);
    }

    public ZscriptCommandBuilder<DefaultResponse> addBigFieldAsSmallest(byte[] data) {
        return super.addBigField(data);
    }

    public ZscriptCommandBuilder<DefaultResponse> addBigField(String data) {
        return super.addBigField(data);
    }

    @Nonnull
    @Override
    public ZscriptCommandNode<DefaultResponse> build() {
        return new ZscriptCommandNode<>() {
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
