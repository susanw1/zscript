package net.zscript.javaclient.commandbuilder.defaultCommands;

import net.zscript.javaclient.commandbuilder.commandnodes.ZscriptCommandBuilder;
import net.zscript.javaclient.commandbuilder.commandnodes.ZscriptCommandNode;
import net.zscript.tokenizer.ZscriptExpression;

public class GenericCommandBuilder extends ZscriptCommandBuilder<DefaultResponse> {
    public ZscriptCommandBuilder<DefaultResponse> setField(byte key, int value) {
        return super.setField(key, value);
    }

    public ZscriptCommandBuilder<DefaultResponse> setField(char key, int value) {
        return super.setField(key, value);
    }

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

    @Override
    public ZscriptCommandNode<DefaultResponse> build() {
        return new ZscriptCommandNode<DefaultResponse>() {
            @Override
            public DefaultResponse parseResponse(ZscriptExpression response) {
                return new DefaultResponse(response);
            }

            @Override
            public Class<DefaultResponse> getResponseType() {
                return DefaultResponse.class;
            }
        };
    }
}
