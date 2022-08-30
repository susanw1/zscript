package org.zcode.javaclient.builders;

import java.util.HashMap;
import java.util.Map;
import java.util.Optional;

public class Response {
    private final String responseText;

    private final Map<Character, Integer> params;

    public Response(String responseText) {
        this.responseText = responseText;
        this.params = new HashMap<>();
    }

    public <T> T getResponseParamByKey(Character symbol, Class<T> type) {
        return type.cast(Optional.of(params.get(symbol)));
    }
}
